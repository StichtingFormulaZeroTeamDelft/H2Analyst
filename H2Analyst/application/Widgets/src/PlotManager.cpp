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
AbstractPlot* PlotManager::createPlot() {
	AbstractPlot* plot = new AbstractPlot(this);
	m_Plots.push_back(plot);
	connect(plot, SIGNAL(timeAxisChanged(AbstractPlot*)), this, SLOT(timeAxisChanged(AbstractPlot*)));
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
	// To avoid this, their parent is changed to be this manager so they can be re-used later.
	for (const auto& plot : m_Plots) plot->setParent(this);

	// Delete all splitters
	delete m_VSplitter;
	m_HSplitters.clear();
	m_VSplitter = new QSplitter(Qt::Vertical, this);
	this->layout()->addWidget(m_VSplitter);

	// Delete empty plots from memory
	auto plotIt = m_Plots.begin();
	while (plotIt != m_Plots.end()) {
		if ((*plotIt)->isEmpty()) { delete* plotIt; plotIt = m_Plots.erase(plotIt); }
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
			if (m_Plots.size() >= plot_counter + 1)
			{
				HSplitter->addWidget(m_Plots[plot_counter]);
				m_Plots[plot_counter]->show();
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
	if (m_Plots.size() > rows * cols)
		m_Plots.erase(m_Plots.begin() + rows * cols, m_Plots.end());
}

/**
* Slot that can be called to align plots to match the given reference plot.
* If no reference is supplied, align plots to match the one with currently the smallest data range.
* 
* @param reference Plot to align time axis of all plots to (dafault = nullptr).
**/
void PlotManager::alignTimeAxis(AbstractPlot* ref) {
	
	// Avoid aligning time axis when a plot is cleared
	if (ref != nullptr) { if (ref->isEmpty()) return; }

	if (!m_BusyAligning) {
		// Setting ranges of other plots causes this slots to be called recursively.
		// m_BusyAligning is used to ignore recursive calls to this function.
		m_BusyAligning = true;

		// If no ref is given, find the plot with the smallest time axis range and use that to set the others.
		if (ref == nullptr) {
			QCPRange range(QCPRange::minRange, QCPRange::maxRange);
			for (const auto& plot : m_Plots) {
				if (plot->type() == H2A::Time && plot->xAxis->range().size() < range.size()) {
					range = plot->xAxis->range();
					ref = plot;
				}
			}
			if (ref == nullptr) ref = m_Plots.front(); // Should never be reached, but to avoid errors.
		}

		// Set plots to match the reference.
		for (const auto& plot : m_Plots) {
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
	if (m_TimeAlignEnabled) this->alignTimeAxis();
}

/**
* Function that reset the views of all plots in this manager.
**/
void PlotManager::resetAllViews() {
	for (const auto& plot : m_Plots)
		if (!plot->isEmpty()) plot->resetView();
}

/**
* Function that return true if all plots in this manager are empty.
**/
bool PlotManager::allPlotsEmpty() {
	for (const auto& plot : m_Plots)
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
	for (const auto& plot : m_Plots) plot->setTimeCursor(time);
	emit this->timeCursorMoved(m_TimeCursorTime);
}

/**
* Function to enable or disable the time cursors in the plots of this manager.
*
* @param enabled Enable time cursors.
**/
void PlotManager::setTimeCursorEnabled(bool enabled) {
	m_TimeCursorEnabled = enabled;
	for (const auto& plot : m_Plots)
		plot->enableTimeCursor(enabled);
}

/**
* Slot that is called when a time axis of any plot is changed.
* 
* @param source Plot that triggered the call of this slot.
**/
void PlotManager::timeAxisChanged(AbstractPlot* source) {
	if (m_TimeAlignEnabled)
		this->alignTimeAxis(source);
}

/**
* When this slot is called, it removes the given widget from the layout and deletes it.
**/
void PlotManager::deletePlot(AbstractPlot* source) {
	if (source == nullptr) return;

	// Check if source is not the last one.
	if (m_Plots.size() < 2) {
		H2A::Dialog::message("Plotting data becomes quite hard without a single plot...");
		return;
	}

	// Hide plot so layout can adapt and delete it from memory.
	source->hide();
	m_Plots.erase(std::remove(m_Plots.begin(), m_Plots.end(), source), m_Plots.end());
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
bool PlotManager::getOtherTimeRange(AbstractPlot* source, QCPRange& range) const {
	if (!m_TimeAlignEnabled) return false;
	for (const auto& plot : m_Plots) {
		if (plot != source && plot->type() == H2A::Time) {
			range = plot->xAxis->range();
			return true;
		}
	}
	return false;
}