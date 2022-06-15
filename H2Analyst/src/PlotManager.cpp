#include "PlotManager.h"

PlotManager::PlotManager(QWidget* parent) : QWidget(parent),
m_VLayout(new QVBoxLayout),
m_DataPanel(nullptr),
m_Empty(true),
m_AlignTimeAxis(true),
m_BusyAligning(false),
m_TimeRange(0.0, 10.0)
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

PlotWidget* PlotManager::createPlot()
{
	PlotWidget* plot = new PlotWidget(this);
	connect(plot, SIGNAL(timeRangeChanged(PlotWidget*)), this, SLOT(plotTimeAxisChanged(PlotWidget*)));
	connect(plot, &PlotWidget::resetViewsRequested, [=]() { this->resetViews(); });
	connect(plot, SIGNAL(dataAdded(PlotWidget*)), this, SLOT(plotDataAdded(PlotWidget*)));
	connect(plot, SIGNAL(dataCleared(PlotWidget*)), this, SLOT(plotDataRemoved(PlotWidget*)));
	m_Plots.push_back(plot);
	return plot;
}

void PlotManager::setDataPanel(const DataPanel* datapanel)
{
	m_DataPanel = datapanel;
	for (const auto& plot : m_Plots)
	{
		plot->setDataPanel(m_DataPanel);
	}
}

// This function changes the plot layout via the plot layout dialog
void PlotManager::setPlotLayoutDialog()
{
	DialogPlotLayout dialog(this);
	dialog.exec();
	if (dialog.result() == QDialog::Accepted)
	{
		this->setPlotLayoutRC(dialog.m_SelectedLayout.front(), dialog.m_SelectedLayout.back());
	}
}

// Used to set plot layout directly with a given number of rows and colums
void PlotManager::setPlotLayoutRC(uint8_t rows, uint8_t cols)
{
	// Delete current layouts and remove plots (does not delete plots from memory, only hides them to re-use later)
	this->clearLayout(m_VLayout);

	// Delete empty plots from memory
	m_Plots.erase(std::remove_if(m_Plots.begin(), m_Plots.end(), [](PlotWidget* plot) { return plot->isEmpty(); }), m_Plots.end());


	// Add new horizontal layouts with empty plots
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

	// Set datapanel
	this->setDataPanel(m_DataPanel);
}


// Recursive function to delete all layouts and items contained in a given layout
void PlotManager::clearLayout(QLayout* layout) {
	if (layout == NULL)
		return;
	QLayoutItem* item;
	while ((item = layout->takeAt(0))) {
		if (item->layout()) {
			this->clearLayout(item->layout());
			delete item->layout();
		}
		else if (item->widget()) {
			// Remove widget from layout and hide it.
			layout->removeWidget(item->widget());
			item->widget()->hide();
		}
	}
	delete item;
}


void PlotManager::plotTimeAxisChanged(PlotWidget* source)
{
	if (source->isEmpty()) return;

	if (m_AlignTimeAxis && !m_BusyAligning)
	{
		m_BusyAligning = true;
		m_TimeRange = source->xAxis->range();

		// Change all plots
		for (const auto& plot : m_Plots)
		{
			if (plot == source || plot->isEmpty()) continue;
			plot->xAxis->setRange(m_TimeRange);
			plot->replot();
		}

		m_BusyAligning = false;
	}
}


void PlotManager::alignTimeAxis(bool align)
{
	m_AlignTimeAxis = align;

	if (m_AlignTimeAxis && m_Plots.size() > 0) // m_Plots should always have at least 1 plot, but check it to be sure
	{
		// Find plot that has smallest time range
		PlotWidget* ref = m_Plots.front();
		for (const auto plot : m_Plots)
		{
			if (plot->type() != PlotWidget::PlotType::Time) continue; // Only consider time-based plots
			if (plot->currentRangeX().size() < ref->currentRangeX().size()) ref = plot;
		}

		// Call slot to align other axis
		this->plotTimeAxisChanged(ref);
	}
}


void PlotManager::resetViews()
{
	for (const auto& plot : m_Plots)
	{
		plot->resetView();
	}
}

/**
* Slot that is called when data is added from any of the plots in this manager.
* 
* @param source PlotWidget where signal was triggered from.
**/
void PlotManager::plotDataAdded(PlotWidget* source)
{
	std::cout << m_TimeRange.lower << " - " << m_TimeRange.upper << std::endl;

	// Set empty variable
	m_Empty = true;
	for (const auto& plot : m_Plots)
		if (!plot->isEmpty()) m_Empty = false;

	if (m_AlignTimeAxis && source->type() == PlotWidget::PlotType::Time)
	{
		source->xAxis->setRange(m_TimeRange);
		source->replot();
	}

}

/**
* Slot that is called when data is removed from any of the plots in this manager.
*
* @param source PlotWidget where signal was triggered from.
**/
void PlotManager::plotDataRemoved(PlotWidget* source)
{
	// Set empty variable
	m_Empty = true;
	for (const auto& plot : m_Plots)
		if (!plot->isEmpty()) m_Empty = false;

}