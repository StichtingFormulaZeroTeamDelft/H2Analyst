#include "DataPanel.h"

DataPanel::DataPanel(QWidget* parent) : QWidget(parent),
m_SearchBox(new QLineEdit(this)),
m_Layout(new QVBoxLayout(this)),
m_TreeView(new TreeView(this)),
m_TreeItemModel(new QStandardItemModel(this)),
m_TreeProxyModel(new QSortFilterProxyModel(this))
{

	m_SearchBox->setPlaceholderText(tr("Find..."));
	m_SearchBox->setClearButtonEnabled(true);
	connect(m_SearchBox, &QLineEdit::returnPressed, [=]() {this->applyFindFilter(); });
	connect(m_SearchBox, SIGNAL(textChanged(QString)), this, SLOT(searchInputChanged()));

	m_TreeItemModel->setSortRole(H2A::ItemRole::kSorting);
	m_TreeProxyModel->setSourceModel(m_TreeItemModel);
	m_TreeProxyModel->setFilterRole(H2A::ItemRole::kFilter);
	m_TreeProxyModel->setRecursiveFilteringEnabled(true);
	m_TreeSelectionModel = new QItemSelectionModel(m_TreeProxyModel);

	m_TreeView->setHeaderHidden(true);
	m_TreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_TreeView->setModel(m_TreeProxyModel);

	m_TreeView->setSelectionModel(m_TreeSelectionModel);

	m_Layout->addWidget(m_SearchBox);
	m_Layout->addWidget(m_TreeView);

	this->setLayout(m_Layout);
}

/**
* Datastore setter
* 
* @param datastore New DataStore
**/
void DataPanel::setDataStore(DataStore* datastore) {
	// Todo: make sure it is safe to change datastore after one has already been connected
	m_DataStore = datastore;
}

/**
* Function to find the Dataset object based on the given QStandardItem.
* 
* @param item Item to find dataset of.
**/
const H2A::Dataset* DataPanel::getDatasetFromItem(const QStandardItem* item) const {
	return static_cast<const H2A::Dataset*>(item->data(H2A::ItemRole::kDatasetPtr).value<const void*>());
}

/**
* Returns the Dataset objects of the item that are currently selected.
**/
std::vector<const H2A::Dataset*> DataPanel::getSelectedDatasets() const {
	QModelIndexList indices = m_TreeSelectionModel->selectedIndexes();

	std::vector<const H2A::Dataset*> datasets;
	for (auto const& index : indices) {
		QStandardItem* item = m_TreeItemModel->itemFromIndex(m_TreeProxyModel->mapToSource(index));
		this->addChildrenDatasets(item, datasets);
	}

	return datasets;
}

/**
* Takes the given item and adds its children datasets to the given vector.
* 
* @param item Item to add children datasets of.
* @param target Vector in which children are placed.
**/
void DataPanel::addChildrenDatasets(const QStandardItem* item, std::vector<const H2A::Dataset*>& target) const {
	
	if (item->data(H2A::ItemRole::kItemType).value<H2A::ItemType>() == H2A::ItemType::kDataset) {
		// This is a dataset, so simply add it to the list of datasets
		const H2A::Dataset* item_ds = this->getDatasetFromItem(item);
		// For good measure, make sure the dataset is not already in the target list (should never occur)
		if (std::find(target.begin(), target.end(), item_ds) == target.end())
			target.push_back(item_ds);
	}

	// Add all children of this item as well
	for (auto i = 0; i < item->rowCount(); ++i)
		this->addChildrenDatasets(item->child(i), target);
}

/**
* Re-generates the item views.
**/
void DataPanel::updateData() {
	m_TreeItemModel->clear();

	QStandardItem *root = m_TreeItemModel->invisibleRootItem();
	for (auto const& df : m_DataStore->getDatafiles())
		root->appendRow(this->createTreeItemFromDatafile(df));

	m_TreeItemModel->sort(0);
}


/**
* Requests priority population for given dataset. Optionally, block until the dataset is finished populating.
* 
* @param dataset Dataset to populate.
* @param blocking Enable blocking until finished populating (default = false)
**/
void DataPanel::requestDatasetPopulation(const H2A::Dataset* dataset, bool blocking) const {
	if (!dataset->populated) m_DataStore->requestDatasetPopulation(dataset);	
	if (!blocking) return;
	while (!dataset->populated);
}

/**
* Requests priority population for given datasets. Optionally, block until the datasets are finished populating.
*
* @param datasets Datasets to populate.
* @param blocking Enable blocking until finished populating (default = false)
**/
void DataPanel::requestDatasetPopulation(std::vector<const H2A::Dataset*> datasets, bool blocking) const {
	for (auto const& dataset : datasets)
		if (!dataset->populated) this->requestDatasetPopulation(dataset, false);	
	if (!blocking) return;
	for (auto const& dataset : datasets)
		while (!dataset->populated);
}

/**
* Function that generates the tree items for a given datafile.
* 
* @param df Datafile to create tree items for.
**/
QStandardItem* DataPanel::createTreeItemFromDatafile(const H2A::Datafile* df) {
	std::vector<std::string> str_split;
	
	// Create item for datafile and set its name
	boost::split(str_split, df->name, boost::is_any_of("."));
	QStandardItem* datafile_item = this->createTreeItem(H2A::ItemType::kDatafile, str_split.front());

	// Create a map with the systems and their belonging datasets
	std::map<std::string, std::vector<H2A::Dataset*>> system_map;
	for (auto const& ds : df->datasets) {
		boost::split(str_split, ds->name, boost::is_any_of("-"));
		std::string str_system = str_split.front();
		boost::trim(str_system);
		if (system_map.find(str_system) == system_map.end())
			system_map[str_system] = std::vector<H2A::Dataset*>();
		system_map[str_system].push_back(ds);
	}

	// Iterate over the created map and create its items
	for (auto const& [system, datasets] : system_map) {
		QStandardItem* system_item = this->createTreeItem(H2A::ItemType::kSystem, system);
		datafile_item->appendRow(system_item);

		// First loop looks for 2-worded subsystems, second loop for 1-worded subsystems
		for (uint8_t n_word = 2; n_word > 0; --n_word) {

			size_t i = 0;
			while (i < datasets.size()) {
				// Check if name is long enough to be part of an N-worded subsystem
				boost::split(str_split, datasets[i]->name, boost::is_any_of(" "));
				if (str_split.size() < n_word + 2) {
					++i;
					continue;
				};

				// Create pattern of first N words of current name
				std::vector<std::string> vec_pattern(str_split.begin(), str_split.begin() + 2 + n_word);
				std::string str_pattern = boost::join(vec_pattern, " ") + " ";

				// Find datasets that match the pattern
				std::vector<H2A::Dataset*> hits;
				for (size_t j = i; j < datasets.size(); ++j) {
					if (datasets[j]->name.size() < str_pattern.size()) continue;
					if (std::string(datasets[j]->name.begin(), datasets[j]->name.begin() + str_pattern.size()) == str_pattern)
						hits.push_back(datasets[j]);
				}

				// If there are at least 3 hits, its enough to make a subsystem
				if (hits.size() > 3) {
					QStandardItem* subsys_item = this->createTreeItem(H2A::ItemType::kSubsystem, boost::join(std::vector<std::string>(vec_pattern.begin() + 2, vec_pattern.begin() + 2 + n_word), " "));
					system_item->appendRow(subsys_item);

					// Iterate over hits and add them to the tree
					for (auto const& ds : hits) {
						boost::split(str_split, ds->name, boost::is_any_of(" "));						
						subsys_item->appendRow(this->createTreeItem(H2A::ItemType::kDataset, boost::join(std::vector<std::string>(str_split.begin() + 2 + n_word, str_split.end()), " "), ds));

						// Remove added datasets from map to avoid adding them more than once
						auto ds_iterator = std::find(system_map[system].begin(), system_map[system].end(), ds);
						if (ds_iterator != datasets.end()) system_map[system].erase(ds_iterator);
					}
				}
				else { ++i; }
			}
		}

		// Any datasets that are still in the system_map do no belong to any sybsystem
		for (auto const& ds : datasets) {
			boost::split(str_split, ds->name, boost::is_any_of(" "));
			system_item->appendRow(this->createTreeItem(H2A::ItemType::kDataset, boost::join(std::vector<std::string>(str_split.begin() + 2, str_split.end()), " "), ds));
		}

	}

	return datafile_item;
}

/**
* Function that creates an item of given type with given name.
* If the type is a dataset, the pointer to this dataset should be supplied as well.
* 
* @param type Type of item to create.
* @param name Name to give the item.
* @param ds Dataset that this item belongs to (default = nullptr).
**/
QStandardItem* DataPanel::createTreeItem(const H2A::ItemType& type, const std::string& name, const H2A::Dataset* ds) {
	QStandardItem* item = new QStandardItem(QString(name.c_str()));
	item->setEditable(false);
		
	// Store item type
	item->setData(QVariant::fromValue(type), H2A::ItemRole::kItemType);

	// Store pointer to dataset if this is a dataset item
	QVariant dataset_ptr;
	if (type == H2A::ItemType::kDataset)
		dataset_ptr = QVariant::fromValue(static_cast<const void*>(ds));
	else
		dataset_ptr = QVariant::fromValue(static_cast<const void*>(nullptr));
	item->setData(dataset_ptr, H2A::ItemRole::kDatasetPtr);

	// Type specific operations
	std::vector<std::string> str_split;
	std::stringstream ttStream, nameStream;
	
	ttStream << "<p style = 'white-space:pre'>";
	QString sort_name(name.c_str());

	switch (type)
	{
	case H2A::ItemType::kDatafile:
		sort_name = "AAAA_" + sort_name;
		item->setData("", H2A::ItemRole::kFilter);

		// Format name to something smaller
		/*
		boost::split(str_split, name, boost::is_any_of("-"));
		for (const auto& str : std::vector<std::string>(str_split.begin() + 2, str_split.end())) {
			nameStream << str;
			if (str != str_split.back()) nameStream << "-";
		}
		nameStream << " " << str_split[1].substr(0, 4) << "-" << str_split[1].substr(4, 2) << "-" << str_split[1].substr(6, 2) << " " <<
			str_split[1].substr(9, 2) << ":" << str_split[1].substr(11, 2) << ":" << str_split[1].substr(13, 2);
		item->setText(QString(nameStream.str().c_str()));
		*/
		item->setText(QString(name.c_str()));

		break;
	case H2A::ItemType::kSystem:
		sort_name = "AAA_" + sort_name;
		item->setData("", H2A::ItemRole::kFilter);
		break;
	case H2A::ItemType::kSubsystem:
		sort_name = "AA_" + sort_name;
		item->setIcon(QIcon(":/icon/dataset"));
		item->setData("", H2A::ItemRole::kFilter);
		break;
	case H2A::ItemType::kDataset:
		sort_name = "A_" + sort_name;
		item->setData(QString(ds->name.c_str()), H2A::ItemRole::kFilter);

		//Tooltip
		ttStream << "<b>Data:</b> " << ds->quantity << " [" << ds->unit << "]\n";
		ttStream << "<b>UID:</b> " << ds->uid << "\n";
		ttStream << "</p>";
		item->setToolTip(ttStream.str().c_str());

		break;
	default: break;
	}

	item->setData(sort_name, H2A::ItemRole::kSorting);

	return item;
}

/**
* Applies the filter from the input box.
**/
void DataPanel::applyFindFilter() {
	QString input = m_SearchBox->text();
	QRegExp regExp(input, Qt::CaseInsensitive, QRegExp::FixedString);
	m_TreeProxyModel->setFilterRegExp(regExp);

	int nDatasets = this->countRows(m_TreeView->rootIndex());
	if (nDatasets <= H2A::TREEVIEW_FILTER_EXPAND_THRESHOLD)
		m_TreeView->expandRecursively(m_TreeView->rootIndex());
}

/**
* Function to apply filter after clear button has been pressed in search box.
**/
void DataPanel::searchInputChanged() {
	if (m_SearchBox->text() == "") this->applyFindFilter();
}

/**
* Function that recursively counts the number of rows (including children) of an index.
* 
* @param index Index of item to count rows of.
**/
int DataPanel::countRows(const QModelIndex& index) {
	int count = 0;
	int rowCount = m_TreeProxyModel->rowCount(index);
	count += rowCount;
	for (int r = 0; r < rowCount; ++r)
		count += this->countRows(m_TreeProxyModel->index(r, 0, index));
	return count;
}

