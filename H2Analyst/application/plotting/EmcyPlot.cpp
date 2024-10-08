#include "EmcyPlot.h"

EmcyPlot::EmcyPlot(const DataPanel* dataPanel, H2A::Car car, QWidget* parent) : AbstractPlot(parent),
m_DataPanel(dataPanel),
m_VLayout(new QVBoxLayout(this)),
m_List(new QListView()),
m_ListModel(new QStandardItemModel(this)),
m_TimeCursor(new TimeCursorItem()),
m_ItemDelegate(nullptr),
m_WarningMsg(new QWidget()),
m_DataMutex(new QMutex())
{
	m_Type = H2A::EmcyList;

	m_TimeCursor->setBackground(QBrush(QColor(Qt::black)));

	// Build warning message
	QHBoxLayout* warningLayout = new QHBoxLayout(m_WarningMsg);
	QLabel* textLbl = new QLabel("WARNING: Static EMCY definitions are being used");
	QLabel* iconLbl = new QLabel();
	iconLbl->setPixmap(QPixmap(":/icons/warning").scaledToWidth(WARNING_ICON_WIDTH, Qt::SmoothTransformation));
	iconLbl->setMinimumWidth(24); // Makes sure icon is always fully shown when window is resized

	warningLayout->addWidget(iconLbl);
	warningLayout->addWidget(textLbl, Qt::AlignLeft | Qt::AlignVCenter);

	m_VLayout->addWidget(m_List);
	m_VLayout->addWidget(m_WarningMsg);

	m_ItemDelegate = new ItemDelegate(m_ListModel, m_TimeCursor, this);
	m_List->setModel(m_ListModel);
	m_ListModel->setSortRole(H2A::ItemRole::Sorting);
	m_List->setItemDelegate(m_ItemDelegate);
	m_ListModel->appendRow(m_TimeCursor);

	// Hide plot elements
	this->xAxis->setVisible(false);
	this->xAxis->grid()->setVisible(false);
	this->yAxis->setVisible(false);
	this->xAxis->grid()->setVisible(false);

	connect(m_List, &QListView::doubleClicked, this, &EmcyPlot::itemDoubleClicked);

	H2A::Emcy::readEmcyCodesFromSettings(m_EmcyProperties, car);
	this->fillList();
}

/**
* Function that gets the emcies from the dataStore via the dataPanel and puts them in the list.
**/
void EmcyPlot::fillList() {
	// Mutex is used to protect the dataset vector
	m_DataMutex->lock();

	if (m_EmcyDatasets.empty()) {
		if (!this->getDatasets()) {
			return;
		}
	}

	m_DataMutex->unlock();

	// Clear list except for the TimeCursor
	m_ListModel->removeRows(0, m_TimeCursor->row());
	m_ListModel->removeRows(1, m_ListModel->rowCount() - 1);

	// Create vector of emcy structs
	std::vector<H2A::Emcy::Emcy> emcies;
	for (const auto& dataset : m_EmcyDatasets) {
		auto timeVec = dataset->timeVec();
		for (auto i = 0; i < timeVec.size(); ++i) {
			H2A::Emcy::Emcy emcy;
			auto payload = static_cast<uint64_t>(dataset->byteVec[i]);
			H2A::Emcy::readPayload(emcy, payload);

			if (m_EmcyProperties.find(emcy.code) != m_EmcyProperties.end()) {
				auto description = m_EmcyProperties[emcy.code];
				if (description.hide) continue; // Don't add the EMCY if it is marked as hidden
				emcy.description = description.text;
			}
			else {
				emcy.description = "unknown";
				std::stringstream message;
				message << "Found an EMCY (" << std::hex << emcy.code << std::dec << ") that is not listed in the emcy_codes settings file.";
				H2A::logWarning(message.str());
			}

			emcy.time = timeVec[i];
			std::vector<std::string> str_split;
			boost::split_regex(str_split, dataset->name, boost::regex("EMCY"));
			emcy.source = "Source: " + StrOps::trim_copy(str_split.back());
			emcies.push_back(emcy);
			
		}
	}

	// Add items
	for (const auto& emcy : emcies)
		this->addItem(emcy);

	// Sort
	this->sort();
}

/**
* Get EMCY datasets from the selected datafiles.
* If multiple datafiles are selected the first file is used.
**/
bool EmcyPlot::getDatasets() {
	// Make sure a datafile is selected (should also be taken care of by emcy plot command)
	auto datafiles = m_DataPanel->getSelectedDatafiles();
	const H2A::Datafile* datafile = nullptr;
	if (datafiles.size() == 0 && m_DataPanel->getDatafiles().size() == 1) {
		datafile = m_DataPanel->getDatafiles().front();
	}
	else {
		datafile = m_DataPanel->getSelectedDatafiles().front();
	}
	if (datafile == nullptr) {
		H2A::logWarning("Failed to select a datafile for emcy list.");
		return false;
	}

	// Gather EMCY datasets and make sure they are populated before going on
	for (const auto& dataset : datafile->datasets) {
		if (dataset->datatype == 10) m_EmcyDatasets.push_back(dataset);
	}
	m_DataPanel->requestDatasetPopulation(m_EmcyDatasets, true);

	return true;
}

/**
* Create list items for a given emcy.
* 
* @param emcy Emcy to create list item for.
**/
void EmcyPlot::addItem(const H2A::Emcy::Emcy& emcy) {
	
	// Create text label based on emcy data
	std::stringstream ss;
	ss << "[" << std::fixed << std::setprecision(3) << emcy.time << "]\t(" << H2A::Emcy::getSeverityStr(emcy.severity) << ")\t" << emcy.description;
	
	auto item = new QStandardItem(tr(ss.str().c_str()));
	item->setData(QVariant().fromValue<H2A::Emcy::Emcy>(emcy), H2A::ItemRole::Emcy);
	item->setData(emcy.time + 1e-6, H2A::ItemRole::Sorting); // Add 1e-6 to sorting time to make sure cursor is always in front
	item->setToolTip(QString(emcy.source.c_str()));
	item->setEditable(false);
	m_ListModel->appendRow(item);

}

/**
* Override for resizeEvent to make sure list always has the same size as underlying widget.
**/
void EmcyPlot::resizeEvent(QResizeEvent* event) {
	this->resize(this->size());
	QCustomPlot::resizeEvent(event);
}

/**
* Slot that is connected to a double click signal of list items.
* 
* @param event Event that caused this slot to be called.
**/
void EmcyPlot::itemDoubleClicked(const QModelIndex& index) {
	auto item = m_ListModel->itemFromIndex(index);
	H2A::Emcy::Emcy emcy = item->data(H2A::ItemRole::Emcy).value<H2A::Emcy::Emcy>();
	emit this->timeCursorPlaced(emcy.time);
}

/**
* Slots that places the time cursor.
* 
* @param time Time to set the cursor to.
**/
void EmcyPlot::setTimeCursorTime(double time) {
	m_TimeCursor->setData(time, H2A::ItemRole::Sorting);
	this->sort();
}

/**
* Slots that enables/disables the time cursor.
* 
* @param enabled Flag that enables time cursor if true.
**/
void EmcyPlot::setTimeCursorEnabled(bool enabled) {
	m_List->setRowHidden(m_TimeCursor->row(), !enabled);
}

/**
* Sort the emcy list.
**/
void EmcyPlot::sort() {
	m_ListModel->sort(0);
}

/**
* Slot that sets the currently selected car.
* Connected to signal from PlotManager.
* Whenever the selected car is changed the EmcyProperties should be reloaded.
* 
* @param car The currently selected car.
**/
void EmcyPlot::setSelectedCar(H2A::Car car) {
	H2A::Emcy::readEmcyCodesFromSettings(m_EmcyProperties, car);
	this->fillList();
}

/**
* Custom delegate to change the height of a QStandardItem.
* Used to make the TimeCursor much less high.
**/
ItemDelegate::ItemDelegate(QStandardItemModel* model, TimeCursorItem* timeCursor, QObject* parent) : QStyledItemDelegate(parent),
m_Model(model),
m_TimeCursor(timeCursor) {}

/**
* Custom delegate to make time cursor item only a single pixel in height.
**/
QSize ItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QSize s = QStyledItemDelegate::sizeHint(option, index);
	auto item = m_Model->itemFromIndex(index);
	if (m_Model->itemFromIndex(index)->row() == m_TimeCursor->row()) {
		s.setHeight(1);
	}
	return s;
}
