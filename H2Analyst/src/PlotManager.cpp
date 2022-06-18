#include "PlotManager.h"

PlotManager::PlotManager(QWidget* parent) : QWidget(parent),
m_VLayout(new QVBoxLayout),
m_DataPanel(nullptr),
m_AlignTimeAxisEnabled(true),
m_BusyAligning(false),
m_TimeCursorTime(0.0)
{
	// Create layouts
	QHBoxLayout *HLayout = new QHBoxLayout();
	m_HLayouts.push_back(HLayout);
	m_VLayout->addLayout(m_HLayouts.back());

	// Init single plot
	PlotWidget* plot = this->createPlot();
	m_HLayouts.back()->addWidget(plot);

	this->setLayout(m_VLayout);
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
	// Delete current layouts and remove plots (does not delete plots from memory, only hides them to re-use later)
	this->clearLayout(m_VLayout);

	// Delete empty plots from memory
	m_Plots.erase(std::remove_if(m_Plots.begin(), m_Plots.end(), [](PlotWidget* plot) { return plot->isEmpty(); }), m_Plots.end());

	// Create new layout and fill it with existing or new plots
	uint8_t plot_counter = 0;
	for (uint8_t row_i = 0; row_i < rows; ++ row_i)
	{
		QHBoxLayout* layout = new QHBoxLayout();
		for (uint8_t col_i = 0; col_i < cols; ++col_i)
		{
			// Use existing plot if possible, otherwise make a new one
			if (m_Plots.size() >= plot_counter + 1)
			{
				layout->addWidget(m_Plots[plot_counter]);
				m_Plots[plot_counter]->show();
			}
			else {
				PlotWidget* plot = this->createPlot();
				layout->addWidget(plot);
			}
			++plot_counter;
		}
		m_VLayout->addLayout(layout);
	}

	// Delete plots that don't fit new layout from memory
	if (m_Plots.size() > rows * cols)
		m_Plots.erase(m_Plots.begin() + rows * cols, m_Plots.end());
}


/**
* Recursive function to delete all layouts and items contained in a given layout.
* 
* @param layout Layout to clear.
**/
void PlotManager::clearLayout(QLayout* layout) {
	if (layout == nullptr) return;

	QLayoutItem* item;
	while ((item = layout->takeAt(0))) {
		if (item->layout()) {
			this->clearLayout(item->layout());
			delete item->layout();
		}
		else if (item->widget()) {
			layout->removeWidget(item->widget());
			item->widget()->hide();
		}
	}
	delete item;
}

/**
* Slot that can be called to align plots to match the given reference plot.
* If no reference is supplied, align plots to match the one with currently the smallest data range.
* 
* @param reference Plot to align time axis of all plots to (dafault = nullptr).
**/
void PlotManager::alignTimeAxis(PlotWidget* ref) {
	
	if (!m_BusyAligning && !ref->isEmpty())
	{
		// Setting ranges of other plots causes this slots to be called recursively.
		// m_BusyAligning is used to ignore the calls that will be caused by this original call.
		m_BusyAligning = true;

		for (const auto& plot : m_Plots) {
			// Check if plot should be time-aligned
			if (plot->isEmpty() || !plot->timeAxisAlignable() || plot == ref) continue;
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
	if (m_AlignTimeAxisEnabled) {

		//this->alignTimeAxis();
	}
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
}

/**
* Function to enable or disable the time cursors in the plots of this manager.
*
* @param enabled Enable time cursors.
**/
void PlotManager::setTimeCursorEnabled(bool enabled) {
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
* This function is used by plots to get the time axis range of a different plot in the manager.
* When using an empty plot, it needs this to set the time range to match the other when time alignment is enabled.
* 
* @param source Plot that is requesting a time range from a different one.
* @param range Range object to store result in.
**/
bool PlotManager::getOtherTimeRange(PlotWidget* source, QCPRange& range) const {
	if (!m_AlignTimeAxisEnabled) return false;
	for (const auto& plot : m_Plots) {
		if (plot != source && !plot->isEmpty() && plot->timeAxisAlignable()) {
			range = plot->xAxis->range();
			return true;
		}
	}
	return false;
}