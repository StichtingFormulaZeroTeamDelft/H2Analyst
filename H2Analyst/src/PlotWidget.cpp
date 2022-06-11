#include "PlotWidget.h"


PlotWidget::PlotWidget(QWidget* parent) : QCustomPlot(parent),
m_Type(PlotType::Time),
m_DataPanel(nullptr),
m_Plottables(),
m_RangeX(0.0, 10.0),
m_RangeY(0.0, 10.0),
m_PaddingX(0.0),
m_PaddingY(0.0),
m_LegendEnabled(true)
{
	this->setMouseTracking(true); // Should be default for QCustomPlot, but to be sure
	this->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // Sets comma as thousand-seperator and period as decimal-point.
	this->setNoAntialiasingOnDrag(true);

	// Cursor crosshairs
	m_Crosshairs = new Crosshairs(this, QString("crosshairs"), this->layer("axes"));
	m_Crosshairs->enable();

	// Set ranges and labels
	this->clear();

	// Right-click menu
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

	// User interactions
	this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iMultiSelect);
	this->setAcceptDrops(true);

	connect(this->xAxis, SIGNAL(rangeChanged(const QCPRange&)), this, SLOT(emitTimeRangeChanged()));
	connect(this->xAxis, SIGNAL(rangeChanged(const QCPRange&)), this, SLOT(enforceAxisLimits()));
	connect(this->yAxis, SIGNAL(rangeChanged(const QCPRange&)), this, SLOT(enforceAxisLimits()));

}

bool PlotWidget::isEmpty()
{
	return m_Plottables.size() == 0;
}

void PlotWidget::setDataPanel(const DataPanel* datapanel) { m_DataPanel = datapanel; }

void PlotWidget::setPlots(std::vector<const H2A::Dataset*> datasets, bool replot)
{
	if (datasets.size() == 0) return;
	this->clear();
	this->addPlots(datasets, replot);
}

void PlotWidget::addPlots(std::vector<const H2A::Dataset*> datasets, bool replot)
{
	switch (m_Type)
	{
	case PlotType::Time:
	{
		for (auto const& dataset : datasets)
		{
			// Check if this dataset is not already plotted
			bool alreadyPlotted = false;
			for (auto const& plottable : m_Plottables)
			{
				if (dataset == plottable->datasets().front())
				{
					alreadyPlotted = true;
					break;
				}
			}
			if (alreadyPlotted) continue;

			if (!dataset->populated) m_DataPanel->requestDatasetPopulation(dataset, true);

			TimeSeries* ts = new TimeSeries(this, dataset);
			ts->setColor(k_PlotColors[m_Plottables.size() % k_PlotColors.size()]);
			m_Plottables.push_back(ts);
		}
		break;
	}
	case PlotType::XY:
	{
		break;
	}
	}

	if (replot) this->plot();
}


void PlotWidget::plotSelected()
{
	this->setPlots(m_DataPanel->getSelectedDatasets(), true);
}

// Set plot properties based on the plottables that have been added to it.
void PlotWidget::plot()
{
	// Determine data range
	double xmin = 0.0, xmax = 0.0, ymin = 0.0, ymax = 0.0;
	for (const auto& plotLine : m_Plottables)
	{
		xmin = std::min({ xmin, plotLine->rangeX().lower });
		xmax = std::max({ xmax, plotLine->rangeX().upper});
		ymin = std::min({ ymin, plotLine->rangeY().lower});
		ymax = std::max({ ymax, plotLine->rangeY().upper});
	}

	this->setAxisLabels();

	// Store and set axis ranges and calculate paddings
	m_RangeX = QCPRange(xmin, xmax);
	m_RangeY = QCPRange(ymin, ymax);
	if (m_RangeY.lower > 0.0) m_RangeY.lower = 0.0; // This (almost) always makes sense for visualization
	m_PaddingX = (m_RangeX.upper - m_RangeX.lower) * RANGE_PADDING_X;
	m_PaddingY = (m_RangeY.upper - m_RangeY.lower) * RANGE_PADDING_Y;

	this->resetView();
	
}

void PlotWidget::clear()
{
	// Remove plottables
	for (const auto& plottable : m_Plottables)
		delete plottable;
	m_Plottables.clear();

	// Reset axis and labels
	this->xAxis->setLabel("");
	this->yAxis->setLabel("");
	m_RangeX = QCPRange(0.0, 10.0);
	m_RangeY = QCPRange(0.0, 10.0);
	this->xAxis->setRange(m_RangeX);
	this->yAxis->setRange(m_RangeY);
}

void PlotWidget::setAxisLabels()
{
	switch (m_Type)
	{
	case PlotType::Time:
	{
		// Create unique list of quantities of datasets
		std::vector<std::string> units;
		for (const auto& plottable : m_Plottables)
		{
			if (std::find(units.begin(), units.end(), plottable->datasets().front()->unit) == units.end())
			{
				units.push_back(plottable->datasets().front()->unit);
			}
		}

		std::stringstream label;
		if (units.size() < 2)
		{
			// Since only a unique unit was found, we can assume there is also only one unique quantity
			label << m_Plottables.front()->datasets().front()->quantity << " [" << units.front() << "]";
		}
		else
		{
			label << "Mixed quantities [";
			for (size_t i = 0; i < units.size(); ++i)
			{
				label << units[i];
				if (i < units.size() - 1) label << ", ";
			}
			label << "]";
		}
		this->yAxis->setLabel(QString(label.str().c_str()));
		this->xAxis->setLabel("Time [sec]");
	}
	case PlotType::XY:
	{

	}
	}
}


void PlotWidget::showContextMenu(const QPoint& pos)
{
	QMenu contextMenu(tr("Context menu"), this);

	QAction acPlot("Plot", this);
	connect(&acPlot, SIGNAL(triggered()), this, SLOT(plotSelected()));
	contextMenu.addAction(&acPlot);

	QAction acClear("Clear", this);
	connect(&acClear, SIGNAL(triggered()), this, SLOT(clear()));
	contextMenu.addAction(&acClear);

	QAction acReset("Reset view", this);
	connect(&acReset, &QAction::triggered, [=]() {this->resetView(); });
	contextMenu.addAction(&acReset);

	QAction acResetAll("Reset all views", this);
	connect(&acResetAll, SIGNAL(triggered()), this, SIGNAL(resetViewsRequested()));
	contextMenu.addAction(&acResetAll);

	QAction acClip("Clip", this);
	connect(&acClip, SIGNAL(triggered()), this, SLOT(copyToClipboard()));
	contextMenu.addAction(&acClip);

	contextMenu.exec(mapToGlobal(pos));
}


void PlotWidget::copyToClipboard()
{
	QApplication::clipboard()->setPixmap(this->grab());
}


void PlotWidget::dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
}


void PlotWidget::dropEvent(QDropEvent* event)
{
	// Datasets are dragged into this plot.
	// If CTRL is pressed, add them to the existing datasets.
	// Otherwise, replace the datasets by the dragged ones.

	if (QApplication::keyboardModifiers() & Qt::ControlModifier)
	{
		this->addPlots(m_DataPanel->getSelectedDatasets(), true);
	}
	else
	{
		this->plotSelected();
	}
}

// Function that makes sure the axis ranges stay within limits to keep data in view
void PlotWidget::enforceAxisLimits()
{
	// Determine padding as fraction of visible range (to keep the screen space padding constant)
	double xPadding = (this->xAxis->range().upper - this->xAxis->range().lower) * RANGE_PADDING_X;
	double yPadding = (this->yAxis->range().upper - this->yAxis->range().lower) * RANGE_PADDING_Y;
	// Make sure this padding is not larger than the absolute max based on the data itself
	xPadding = std::min({ xPadding, m_PaddingX });
	yPadding = std::min({ yPadding, m_PaddingY });

	// X axis lower bound
	if (this->xAxis->range().lower < m_RangeX.lower - xPadding)
		this->xAxis->setRange(m_RangeX.lower - xPadding, this->xAxis->range().upper);
	// X axis upper bound
	if (this->xAxis->range().upper > m_RangeX.upper + xPadding)
		this->xAxis->setRange(this->xAxis->range().lower, m_RangeX.upper + xPadding);
	// Y axis lower bound
	if (this->yAxis->range().lower < m_RangeY.lower - yPadding)
		this->yAxis->setRange(m_RangeY.lower - yPadding, this->yAxis->range().upper);
	// Y axis upper bound
	if (this->yAxis->range().upper > m_RangeY.upper + yPadding)
		this->yAxis->setRange(this->yAxis->range().lower, m_RangeY.upper + yPadding);
}

// Function to reset view to fit all data with padding included
void PlotWidget::resetView()
{
	this->xAxis->setRange(m_RangeX.lower - m_PaddingX, m_RangeX.upper + m_PaddingX);
	this->yAxis->setRange(m_RangeY.lower - m_PaddingY, m_RangeY.upper + m_PaddingY);
	this->replot();
}

void PlotWidget::mouseMoveEvent(QMouseEvent* event)
{
	// Only propagate event is plot is in use (to disable panning and zooming)
	if (!this->isEmpty())
		QCustomPlot::mouseMoveEvent(event);

	emit this->mouseMoved(event);
}

void PlotWidget::wheelEvent(QWheelEvent* event)
{
	if (!this->isEmpty())
		QCustomPlot::wheelEvent(event);
	
	emit this->wheelMoved(event);
}

void PlotWidget::leaveEvent(QEvent* event)
{
	emit this->mouseLeft();
}
