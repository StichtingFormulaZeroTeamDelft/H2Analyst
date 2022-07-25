#include "PlotManager.h"

PlotManager::PlotManager(QWidget* parent) : QWidget(parent),
m_Layout(new QVBoxLayout(this)),
m_VSplitter(new QSplitter(Qt::Vertical, this)),
m_HSplitters(),
m_DataPanel(nullptr),
m_TimeAlignEnabled(true),
m_BusyAligning(false),
m_TimeCursorEnabled(false),
m_TimeCursorTime(0.0)
{
	m_VSplitter->setChildrenCollapsible(false);

	// Init single plot
	this->setPlotLayoutRC(1, 1);

	this->layout()->addWidget(m_VSplitter);
}

/**
* Convenience function for creating a new plot, setting its members and connecting signals.
**/
AbstractPlot* PlotManager::createPlot(H2A::PlotType type) {
	AbstractPlot* plot = new AbstractPlot(this);
	switch (type)
	{
	case H2A::Time:
		plot = new TimePlot(this);
		break;
	case H2A::XY:
		break;
	default:
		break;
	}
	connect(plot, SIGNAL(timeAxisChanged(AbstractPlot*)), this, SLOT(alignTimeAxis(AbstractPlot*)));
	connect(plot, &AbstractPlot::plotSelected, this, &PlotManager::plotSelected);
	connect(plot, &AbstractPlot::contextMenuRequested, this, &PlotManager::contextMenu);

	plot->setTimeCursorEnabled(m_TimeCursorEnabled);
	plot->setTimeCursorTime(m_TimeCursorTime);
	connect(plot, &AbstractPlot::timeCursorPlaced, this, [=]() {this->setTimeCursorEnabled(true); });
	connect(plot, &AbstractPlot::timeCursorPlaced, this, &PlotManager::setTimeCursorTime);
	
	return plot;
}

/**
* Function that creates and executes the dialog for setting the layout.
**/
void PlotManager::setPlotLayoutDialog() {
	DialogPlotLayout dialog(this);
	dialog.exec();
	if (dialog.result() == QDialog::Accepted)
		this->setPlotLayoutRC(dialog.m_SelectedLayout.front(), dialog.m_SelectedLayout.back());
}

/**
* Function that sets the layout of plots to a given number of rows and colums.
* It re-uses plots if they are available from the previous layout. If those do not fill the new layout, create new empty plots.
*
* @param rows Number of rows in new layout.
* @param cols Number of cols in new layout.
**/
void PlotManager::setPlotLayoutRC(uint8_t rows, uint8_t cols) {
	// When plots were added to the layout, the layout became their parent.
	// If the layout would be deleted, the plots are also deleted from memory.
	// To avoid this, get a vector of pointers and set their parents to be this manager so they can be re-used later.
	auto plots = this->plots();
	for (const auto& plot : plots) plot->setParent(this);

	// Delete all splitters
	delete m_VSplitter;
	m_HSplitters.clear();
	m_VSplitter = new QSplitter(Qt::Vertical, this);
	this->layout()->addWidget(m_VSplitter);

	// Delete empty plots from memory
	auto plotIt = plots.begin();
	while (plotIt != plots.end()) {
		if ((*plotIt)->isEmpty()) { delete* plotIt; plotIt = plots.erase(plotIt); }
		else plotIt++;
	}

	// Create new layout and fill it with existing or new plots
	uint8_t plot_counter = 0;
	QList<int> sizesV, sizesH; // Used to reset the widths of plots to be equal
	for (uint8_t row_i = 0; row_i < rows; ++ row_i)
	{
		QSplitter* HSplitter = new QSplitter(Qt::Horizontal);
		HSplitter->setChildrenCollapsible(false);
		m_HSplitters.push_back(HSplitter);
		for (uint8_t col_i = 0; col_i < cols; ++col_i)
		{
			if (row_i == 0) sizesH.push_back(QGuiApplication::primaryScreen()->virtualSize().width());
			// Use existing plot if possible, otherwise make a new one
			if (plots.size() >= plot_counter + 1)
			{
				HSplitter->addWidget(plots[plot_counter]);
				plots[plot_counter]->show();
			}
			else {
				auto plot = this->createPlot();
				HSplitter->addWidget(plot);
			}
			++plot_counter;
		}
		HSplitter->setSizes(sizesH);
		m_VSplitter->addWidget(HSplitter);
		sizesV.push_back(QGuiApplication::primaryScreen()->virtualSize().height());
	}
	m_VSplitter->setSizes(sizesV);

	// Delete plots that don't fit new layout from memory
	if (plots.size() > rows * cols)
		plots.erase(plots.begin() + rows * cols, plots.end());
}

/**
* Slot that can be called to align plots to match the given reference plot.
* If no reference is supplied, align plots to match the one with currently the smallest data range.
* 
* @param reference Plot to align time axis of all plots to (dafault = nullptr).
**/
void PlotManager::alignTimeAxis(AbstractPlot* ref) {
	
	if (!m_TimeAlignEnabled) return;

	// Avoid aligning time axis when a plot is cleared
	if (ref != nullptr) { if (ref->isEmpty()) return; }

	if (!m_BusyAligning) {
		// Setting ranges of other plots causes this slots to be called recursively.
		// m_BusyAligning is used to ignore recursive calls to this function.
		m_BusyAligning = true;

		// If no ref is given, find the plot with the smallest time axis range and use that to set the others.
		auto plots = this->plots();
		if (ref == nullptr) {
			QCPRange range(QCPRange::minRange, QCPRange::maxRange);
			for (const auto& plot : plots) {
				if (plot->type() == H2A::Time && plot->xAxis->range().size() < range.size()) {
					range = plot->xAxis->range();
					ref = plot;
				}
			}
			if (ref == nullptr) ref = plots.front(); // Should never be reached, but to avoid errors.
		}

		// Set plots to match the reference.
		for (const auto& plot : plots) {
			if (!plot->type() == H2A::Time || plot == ref) continue;
			plot->xAxis->setRange(ref->xAxis->range());
			//plot->zoomYToData();
			plot->replot();
		}

		m_BusyAligning = false;
	}
}

/**
* Slot that enabled time axis alignment between the plots in this manager.
*
* @param align Enable time axis alignment.
**/
void PlotManager::setTimeAlignEnabled(bool align) {
	m_TimeAlignEnabled = align;
	if (m_TimeAlignEnabled) {
		H2A::logInfo("Time alignment enabled");
		this->alignTimeAxis();
	}
	else {
		H2A::logInfo("Time alignment disabled");
	}
}

/**
* Function that reset the views of all plots in this manager.
**/
void PlotManager::resetAllViews() {
	for (const auto& plot : this->plots())
		if (!plot->isEmpty()) plot->resetView();
}

/**
* Function that return true if all plots in this manager are empty.
**/
bool PlotManager::allPlotsEmpty() {
	for (const auto& plot : this->plots())
		if (!plot->isEmpty()) return false;
	return true;
}

/**
* Function to set the time of the time cursors of the plots in this manager.
*
* @param time Time to set the time cursor to.
**/
void PlotManager::setTimeCursorTime(double time) {
	m_TimeCursorTime = time;
	for (const auto& plot : this->plots()) plot->setTimeCursorTime(time);
	emit this->timeCursorMoved(m_TimeCursorTime);
}

/**
* Function to enable or disable the time cursors in the plots of this manager.
*
* @param enabled Enable time cursors.
**/
void PlotManager::setTimeCursorEnabled(bool enabled) {
	m_TimeCursorEnabled = enabled;
	for (const auto& plot : this->plots())
		plot->setTimeCursorEnabled(enabled);
}

/**
* When this slot is called, it removes the given widget from the layout and deletes it.
**/
void PlotManager::deletePlot(AbstractPlot* source) {
	if (source == nullptr) return;

	// Check if source is not the last plot.
	if (this->plots().size() < 2) {
		H2A::Dialog::message("Plotting data becomes quite hard without a single plot...");
		return;
	}

	// Hide plot so layout can adapt and delete it from memory.
	source->hide();
	source->deleteLater();
}

/**
* This slot can be called to add plot(s) relative to an existing one.
* 
* @param ref Reference plot.
* @param dir Direction to add the plot, relative to the reference plot.
**/
void PlotManager::insertPlot(AbstractPlot* ref, H2A::Direction dir)
{
	// Find splitter in which the source widget lives
	QSplitter* refSplitter = qobject_cast<QSplitter*>(ref->parent());
	if (refSplitter == nullptr) return; // Should be impossible, but to avoid errors.
	
	// Add the plot
	QList<int> sizes;
	auto plot = this->createPlot();
	if (dir == H2A::up || dir == H2A::down) { 
		QSplitter* splitter = new QSplitter(Qt::Horizontal);
		splitter->addWidget(plot);
		auto refSplitterIndex = std::find(m_HSplitters.begin(), m_HSplitters.end(), refSplitter);
		if (refSplitterIndex == m_HSplitters.end()) return;
		m_HSplitters.insert(refSplitterIndex, splitter);
		uint8_t offset = (dir == H2A::up) ? 0 : 1;
		m_VSplitter->insertWidget(m_VSplitter->indexOf(refSplitter) + offset, splitter);
		for (int i = 0; i < m_VSplitter->count(); ++i) sizes.push_back(QGuiApplication::primaryScreen()->virtualSize().height());
		m_VSplitter->setSizes(sizes);
	}
	else {
		uint8_t offset = (dir == H2A::left) ? 0 : 1;
		refSplitter->insertWidget(refSplitter->indexOf(ref) + offset, plot);
		for (int i = 0; i < refSplitter->count(); ++i) sizes.push_back(QGuiApplication::primaryScreen()->virtualSize().width());
		refSplitter->setSizes(sizes);
	}
}

/**
* This function is used by plots to get the time axis range of a different plot in the manager.
* When using an empty plot, it needs this to set the time range to match the other when time alignment is enabled.
* 
* @param source Plot that is requesting a time range from a different one.
* @param range Range object to store result in.
**/
bool PlotManager::getOtherTimeRange(AbstractPlot* source, QCPRange& range) {
	if (!m_TimeAlignEnabled) return false;
	for (const auto& plot : this->plots()) {
		if (plot != source && plot->type() == H2A::Time) {
			range = plot->xAxis->range();
			return true;
		}
	}
	return false;
}

/**
* Returns a list of the current plots in the manager.
**/
std::vector<AbstractPlot*> PlotManager::plots() {
	std::vector<AbstractPlot*> plots;
	this->addChildrenPlots(m_VSplitter, plots);
	return plots;
}

/**
* Function that recursively gathers all plots from the layouts.
* 
* @param splitter Splitter to get the child plots from.
* @param plots Vector to store pointers to the widgets in.
**/
void PlotManager::addChildrenPlots(QSplitter* splitter, std::vector<AbstractPlot*>& plots) {
	for (int i = 0; i < splitter->count(); ++i) {
		auto child = splitter->widget(i);
		if (qobject_cast<QSplitter*>(child)) {
			this->addChildrenPlots(qobject_cast<QSplitter*>(child), plots);
			continue;
		}
		if (qobject_cast<AbstractPlot*>(child)) {
			plots.push_back(qobject_cast<AbstractPlot*>(child));
			continue;
		}
	}
}

/**
* Replaces an existing plot with a newly created plot of given type.
* 
* @param source Plot to replace.
* @param newType Type of new plot.
**/
AbstractPlot* PlotManager::replacePlot(AbstractPlot* source, H2A::PlotType newType) {
	AbstractPlot* newPlot = createPlot(newType);
	auto parent = qobject_cast<QSplitter*>(source->parent());
	if (!parent) {
		H2A::Dialog::message("Failed to replace plot widget...");
		return source;
	}
	auto oldPlot = parent->replaceWidget(parent->indexOf(source), newPlot);
	oldPlot->deleteLater();
	return newPlot;
}

/**
* When this slot is called, the currently selected datasets are plotted in the target plot.
* If no plot is specified, it selects the first one that is available.
* 
* @param target Plot widget to plot selected data in.
* @param type Type of plot requested
**/
void PlotManager::plotSelected(AbstractPlot* target, H2A::PlotType type, bool clearFirst) {

	// Get selected datasets and make sure they are populated
	auto datasets = m_DataPanel->getSelectedDatasets();
	m_DataPanel->requestDatasetPopulation(datasets, true);

	// No target specified, find the first one that is still empty
	if (!target) {
		for (const auto& plot : this->plots()) {
			if (plot->isEmpty()) target = plot;
		}
		// If no empty plot is found, abort plotting.
		if (!target) {
			H2A::Dialog::message("No empty plots available for plotting.");
			return;
		}
	}

	// Check if target is right type. If not, change it.
	target = target->type() == type ? target : this->replacePlot(target, type);

	target->plot(datasets, clearFirst);
}


/**
* Function that creates the abstract context menu that applies to all plots.
**/
void PlotManager::contextMenu(AbstractPlot* source, const QPoint& pos) {

	QMenu menu(this);

	QAction* acPlot = new QAction("Plot");
	menu.addAction(acPlot);
	connect(acPlot, &QAction::triggered, [=]() { this->plotSelected(source, H2A::Time); });

	QMenu* plotMenu = menu.addMenu(QIcon(QPixmap(":/icons/more-information")), "Other plots");

	QAction* acPlotXY = new QAction("XY");
	acPlotXY->setEnabled(false);
	// connect action
	plotMenu->addAction(acPlotXY);

	QAction* acResetView = new QAction(QIcon(QPixmap(":/icons/uno")), QString("Reset view"));
	connect(acResetView, &QAction::triggered, [=]() {source->resetView(); });
	menu.addAction(acResetView);

	QAction* acResetAllViews = new QAction(QIcon(QPixmap(":/icons/uno")), QString("Reset all views"));
	connect(acResetAllViews, &QAction::triggered, [=]() {this->resetAllViews(); });
	menu.addAction(acResetAllViews);

	QAction* acClear = new QAction(QString("Clear"));
	connect(acClear, &QAction::triggered, [=]() {source->clear(); });
	menu.addAction(acClear);

	QAction* acDelete = new QAction(QIcon(QPixmap(":/icons/remove")), QString("Delete plot"));
	connect(acDelete, &QAction::triggered, [=]() {this->deletePlot(source); });
	menu.addAction(acDelete);

	menu.exec(source->mapToGlobal(pos));
}