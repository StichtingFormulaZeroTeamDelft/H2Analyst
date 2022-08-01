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

	m_TreeItemModel->setSortRole(H2A::ItemRole::Sorting);
	m_TreeProxyModel->setSourceModel(m_TreeItemModel);
	m_TreeProxyModel->setFilterRole(H2A::ItemRole::Filter);
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
* Function to check if a given UID is present in the given datafile.
* 
* @param datafile Datafile to check if UID exists in.
* @param uid UID to check for.
**/
bool DataPanel::datasetPresentUID(const H2A::Datafile* datafile, const uint32_t uid) const {
	return m_DataStore->datasetPresentUID(datafile, uid);
}

/**
* Function to find the Dataset object based on the given QStandardItem.
* 
* @param item Item to find dataset of.
**/
const H2A::Dataset* DataPanel::getDatasetFromItem(const QStandardItem* item) const {
	if (item->data(H2A::ItemRole::ItemType).value<H2A::ItemType>() != H2A::ItemType::Dataset) return nullptr;
	return static_cast<const H2A::Dataset*>(item->data(H2A::ItemRole::DataPtr).value<const void*>());

}


/**
* Returns the Dataset objects of the item(s) that are currently selected.
**/
std::vector<const H2A::Dataset*> DataPanel::getSelectedDatasets() const {
	QModelIndexList indices = m_TreeSelectionModel->selectedIndexes();

	// Iterate over selected items and add them and their children if they are datasets.
	std::vector<const H2A::Dataset*> datasets;
	for (auto const& index : indices) {
		QStandardItem* item = m_TreeItemModel->itemFromIndex(m_TreeProxyModel->mapToSource(index));
		this->addChildrenDatasets(item, datasets);
	}

	return datasets;
}

/**
* Returns the Datafile objects of the item(s) that are currently selected.
**/
std::vector<const H2A::Datafile*> DataPanel::getSelectedDatafiles() const {
	QModelIndexList indices = m_TreeSelectionModel->selectedIndexes();

	// Iterate over selected items and add them to the result vector if they are datafiles.
	std::vector<const H2A::Datafile*> datafiles;
	for (auto const& index : indices) {
		QStandardItem* item = m_TreeItemModel->itemFromIndex(m_TreeProxyModel->mapToSource(index));
		if (item->data(H2A::ItemRole::ItemType).value<H2A::ItemType>() == H2A::ItemType::Datafile) {
			datafiles.push_back(static_cast<const H2A::Datafile*>(item->data(H2A::ItemRole::DataPtr).value<const void*>()));
		}
	}

	return datafiles;
}

/**
* Takes the given item and adds its children datasets to the given vector.
* 
* @param item Item to add children datasets of.
* @param target Vector in which children are placed.
**/
void DataPanel::addChildrenDatasets(const QStandardItem* item, std::vector<const H2A::Dataset*>& target) const {
	
	if (item->data(H2A::ItemRole::ItemType).value<H2A::ItemType>() == H2A::ItemType::Dataset) {
		// This is a dataset, so simply add it to the list of datasets
		const H2A::Dataset* dataset = this->getDatasetFromItem(item);
		// For good measure, make sure the dataset is not already in the target list (should never occur)
		if (std::find(target.begin(), target.end(), dataset) == target.end())
			target.push_back(dataset);
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
	QStandardItem* datafile_item = this->createTreeItem(df, str_split.front());

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
		QStandardItem* system_item = this->createTreeItem(H2A::ItemType::System, system);
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
					QStandardItem* subsys_item = this->createTreeItem(H2A::ItemType::Subsystem, boost::join(std::vector<std::string>(vec_pattern.begin() + 2, vec_pattern.begin() + 2 + n_word), " "));
					system_item->appendRow(subsys_item);

					// Iterate over hits and add them to the tree
					for (auto const& ds : hits) {
						boost::split(str_split, ds->name, boost::is_any_of(" "));						
						subsys_item->appendRow(this->createTreeItem(ds, boost::join(std::vector<std::string>(str_split.begin() + 2 + n_word, str_split.end()), " ")));

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
			system_item->appendRow(this->createTreeItem(ds, boost::join(std::vector<std::string>(str_split.begin() + 2, str_split.end()), " ")));
		}

	}

	return datafile_item;
}

/**
* Function to create a standardItem for a datafile.
* 
* @param datafile Datafile that belongs to this item.
* @param name Display name of the item.
**/
QStandardItem* DataPanel::createTreeItem(const H2A::Datafile* datafile, const std::string& name) {
	auto item = this->createTreeItem(H2A::ItemType::Datafile, name);
	item->setData(QVariant::fromValue(static_cast<const void*>(datafile)), H2A::ItemRole::DataPtr);
	return item;
}

/**
* Function to create a standardItem for a dataset.
*
* @param dataset Dataset that belongs to this item.
* @param name Display name of the item.
**/
QStandardItem* DataPanel::createTreeItem(const H2A::Dataset* dataset, const std::string& name) {
	auto item = this->createTreeItem(H2A::ItemType::Dataset, name);
	item->setData(QVariant::fromValue(QString(dataset->name.c_str())), H2A::ItemRole::Filter);
	item->setData(QVariant::fromValue(static_cast<const void*>(dataset)), H2A::ItemRole::DataPtr);
	std::stringstream ttStream;
	ttStream << "<p style = 'white-space:pre'>";
	ttStream << "<b>UID:</b> " << dataset->uid << "\n";
	ttStream << "</p>";
	item->setToolTip(ttStream.str().c_str());
	return item;
}

/**
* Function to create a generic standardItem (used for systems and subsystems).
*
* @param type Item type.
* @param name Display name of the item.
**/
QStandardItem* DataPanel::createTreeItem(const H2A::ItemType& type, const std::string& name) {
	QStandardItem* item = new QStandardItem(QString(name.c_str()));
	item->setEditable(false);
	item->setData(QVariant::fromValue(type), H2A::ItemRole::ItemType);
	item->setData("", H2A::ItemRole::Filter);
	item->setData(static_cast<const void*>(nullptr), H2A::ItemRole::DataPtr);

	// Type specific operations
	std::vector<std::string> str_split;
	std::stringstream nameStream;
	QString sort_name(name.c_str());

	switch (type)
	{
	case H2A::ItemType::Datafile:
		sort_name = "AAAA_" + sort_name;
		break;
	case H2A::ItemType::System:
		sort_name = "AAA_" + sort_name;
		break;
	case H2A::ItemType::Subsystem:
		sort_name = "AA_" + sort_name;
		item->setIcon(QIcon(":/icon/dataset"));
		break;
	case H2A::ItemType::Dataset:
		sort_name = "A_" + sort_name;
		break;
	default: break;
	}
	item->setData(sort_name, H2A::ItemRole::Sorting);

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
* Slot to apply filter after clear button has been pressed in search box.
**/
void DataPanel::searchInputChanged() {
	if (m_SearchBox->text() == "") this->applyFindFilter();
}

/**
* Function that recursively counts the number of rows (including children) of an index.
* Used to check how many hits remain after filter has been applied.
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

