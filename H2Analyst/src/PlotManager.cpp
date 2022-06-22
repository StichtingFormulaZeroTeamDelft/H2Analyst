#include "PlotManager.h"

PlotManager::PlotManager(QWidget* parent) : QWidget(parent),
m_Layout(new QVBoxLayout(this)),
m_VSplitter(new QSplitter(Qt::Vertical, parent)),
m_HSplitters(),
m_DataPanel(nullptr),
m_AlignTimeAxisEnabled(true),
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
PlotWidget* PlotManager::createPlot() {
	PlotWidget* plot = new PlotWidget(this);
	plot->setDataPanel(m_DataPanel);
	m_Plots.push_back(plot);

	connect(plot, SIGNAL(timeAxisChanged(PlotWidget*)), this, SLOT(timeAxisChanged(PlotWidget*)));
	
	return plot;
}

/**
* Function to set the DataPanel of this manager, as well as all its plots.
**/
void PlotManager::setDataPanel(const DataPanel* datapanel) {
	m_DataPanel = datapanel;
	for (const auto& plot : m_Plots)
		plot->setDataPanel(m_DataPanel);
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
	// Remove all plots from the current vertical splitter and after that, remove the empty splitters.
	// Plots are not deleted at this point, but only given a new parent so they are not deleted when the splitter is deleted.
	for (const auto& splitter : m_HSplitters) {
		while (splitter->count() > 0)
			splitter->widget(0)->setParent(this);
	}
	m_HSplitters.clear();

	// Delete empty plots from memory
	m_Plots.erase(std::remove_if(m_Plots.begin(), m_Plots.end(), [](PlotWidget* plot) { return plot->isEmpty(); }), m_Plots.end());

	// Create new layout and fill it with existing or new plots
	uint8_t plot_counter = 0;
	QList<int> sizesV; // Used to reset the widths of plots to be equal
	for (uint8_t row_i = 0; row_i < rows; ++ row_i)
	{
		QSplitter* HSplitter = new QSplitter(Qt::Horizontal);
		HSplitter->setChildrenCollapsible(false);
		m_HSplitters.push_back(HSplitter);
		QList<int> sizesH; // Used to reset the widths of plots to be equal
		sizesH.clear();
		for (uint8_t col_i = 0; col_i < cols; ++col_i)
		{
			sizesH.push_back(INT_MAX);
			// Use existing plot if possible, otherwise make a new one
			if (m_Plots.size() >= plot_counter + 1)
			{
				HSplitter->addWidget(m_Plots[plot_counter]);
				m_Plots[plot_counter]->show();
			}
			else {
				PlotWidget* plot = this->createPlot();
				HSplitter->addWidget(plot);
			}
			++plot_counter;
		}
		m_VSplitter->addWidget(HSplitter);
		HSplitter->setSizes(sizesH); // Make all plots have equal width
		sizesV.push_back(INT_MAX);
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
void PlotManager::alignTimeAxis(PlotWidget* ref) {
	
	// Avoid aligning time axis when a plot is cleared
	if (ref != nullptr) {
		if (ref->isEmpty()) return;
	}

	if (!m_BusyAligning)
	{
		// Setting ranges of other plots causes this slots to be called recursively.
		// m_BusyAligning is used to ignore the calls that will be caused by this original call.
		m_BusyAligning = true;

		// If no ref is given, find the plot with the smallest time axis range and use that.
		if (ref == nullptr) {
			QCPRange range(QCPRange::minRange, QCPRange::maxRange);
			ref = nullptr;
			for (const auto& plot : m_Plots) {
				if (plot->timeAxisAlignable() && plot->xAxis->range().size() < range.size()) {
					range = plot->xAxis->range();
					ref = plot;
				}
			}
			if (ref == nullptr) ref = m_Plots.front(); // To avoid errors
		}

		// Set plots to match the reference.
		for (const auto& plot : m_Plots) {
			// Check if plot should be time-aligned
			if (!plot->timeAxisAlignable() || plot == ref) continue;
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
void PlotManager::setAlignTimeAxisEnabled(bool align) {
	m_AlignTimeAxisEnabled = align;
	if (m_AlignTimeAxisEnabled) this->alignTimeAxis();
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
bool PlotManager::isEmpty() {
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
	for (const auto& plot : m_Plots)
		plot->timeCursor()->setTime(time);
	emit this->timeCursurTimeChanged(m_TimeCursorTime);
}

/**
* Function to enable or disable the time cursors in the plots of this manager.
*
* @param enabled Enable time cursors.
**/
void PlotManager::setTimeCursorEnabled(bool enabled) {
	m_TimeCursorEnabled = enabled;
	for (const auto& plot : m_Plots)
		plot->timeCursor()->setEnabled(enabled);
}

/**
* Slot that is called when a time axis of any plot is changed.
* 
* @param source Plot that triggered the call of this slot.
**/
void PlotManager::timeAxisChanged(PlotWidget* source) {
	if (m_AlignTimeAxisEnabled)
		this->alignTimeAxis(source);
}

/**
* When this slot is called, it removes the given widget from the layout and deletes it.
**/
void PlotManager::deletePlot(PlotWidget* source) {
	if (source == nullptr) return;

	// Check if source is not the last one.
	if (m_Plots.size() < 2) {
		H2A::Dialog::message("Plotting data becomes quite hard without a single plot...");
		return;
	}

	source->hide();
	m_Plots.erase(std::remove(m_Plots.begin(), m_Plots.end(), source), m_Plots.end());
	source->deleteLater();
}

/**
* This function is used by plots to get the time axis range of a different plot in the manager.
* When using an empty plot, it needs this to set the time range to match the other when time alignment is enabled.
* 
* @param source Plot that is requesting a time range from a different one.
* @param range Range object to store result in.
**/
bool PlotManager::getOtherTimeRange(PlotWidget* source, QCPRange& range) const {
	if (!m_AlignTimeAxisEnabled) return false;
	for (const auto& plot : m_Plots) {
		if (plot != source && plot->timeAxisAlignable()) {
			range = plot->xAxis->range();
			return true;
		}
	}
	return false;
}