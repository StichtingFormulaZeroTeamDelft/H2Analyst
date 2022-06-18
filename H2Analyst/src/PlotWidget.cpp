#include "PlotWidget.h"


PlotWidget::PlotWidget(PlotManager* plotManager) : QCustomPlot(plotManager),
m_PlotManager(plotManager),
m_Type(PlotType::Time),
m_DataPanel(nullptr),
m_Plottables(),
m_RangeLimitX(0.0, 10.0),
m_RangeLimitY(0.01, 10.0),
m_LegendEnabled(true)
{
	this->setMouseTracking(true); // Should be default for QCustomPlot, but to be sure
	this->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // Sets comma as thousand-seperator and period as decimal-point.

	// Cursor crosshairs
	m_Crosshairs = new Crosshairs(this);
	m_Crosshairs->enable();

	// Time cursor
	m_TimeCursor = new TimeCursor(this);
	m_TimeCursor->setEnabled(m_PlotManager->timeCursorEnabled());
	m_TimeCursor->setTime(m_PlotManager->timeCursorTime());

	// Right-click menu
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

	// User interactions
	this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iMultiSelect);
	this->setAcceptDrops(true);

	// Other signals and slots
	connect(this->xAxis, SIGNAL(rangeChanged(const QCPRange&)), this, SLOT(emitTimeAxisChanged()));
	connect(this->xAxis, SIGNAL(rangeChanged(const QCPRange&, const QCPRange&)), this, SLOT(restrictViewX(const QCPRange&, const QCPRange&)));
	connect(this->yAxis, SIGNAL(rangeChanged(const QCPRange&, const QCPRange&)), this, SLOT(restrictViewY(const QCPRange&, const QCPRange&)));

	this->replot();

}

/**
* Function that clears the current plots and adds the supplied list of datasets as plots of given type.
* 
* @param datasets Vector of datasets to add.
* @param type Type of plot to create.
**/
void PlotWidget::setPlots(std::vector<const H2A::Dataset*> datasets, PlotType type) {
	if (datasets.size() == 0) return;
	if (!this->isEmpty()) this->clear();
	this->addPlots(datasets, type);
}

/**
* Main function that is used to add plots to this widget.
* 
* @param datasets Datasets to add.
* @param type Type of plot to create.
**/
void PlotWidget::addPlots(const std::vector<const H2A::Dataset*> datasets, PlotType type) {
	// If this call is adding plots to existing plots, check if requested type is the same.
	if (type != m_Type && m_Plottables.size() > 0) {
		H2A::Dialog::message("This plot already contains plots of a different type.");
		return;
	}
	else { m_Type = type; }

	// Make sure datasets are populated
	m_DataPanel->requestDatasetPopulation(datasets, true);

	// Check if empty before adding the datasets
	bool wasEmpty = this->isEmpty();

	// Add plots
	switch (m_Type)
	{
	case PlotType::Time:
	{
		for (auto const& dataset : datasets) {
			// Check if this dataset is not already plotted
			bool alreadyPlotted = false;
			for (auto const& plottable : m_Plottables) {
				if (dataset == plottable->datasets().front()) {
					alreadyPlotted = true;
					break;
				}
			}
			if (alreadyPlotted) continue;

			// Create the time series and add to the widget
			TimeSeries* ts = new TimeSeries(this, dataset);
			ts->setColor(this->nextColor());
			m_Plottables.push_back(ts);
		}
		break;
	}
	case PlotType::XY:
	{
		XYSeries* series = new XYSeries(this, datasets);
		series->setColor(this->nextColor());
		m_Plottables.push_back(series);
		break;
	}
	}

	this->setAxisLabels();
	this->updateRangeAndPadding();

	// Setting the axis to correct ranges
	QCPRange range = m_RangeLimitX;
	if (m_PlotManager->getOtherTimeRange(this, range)) {
		this->xAxis->setRange(range);
		this->resetView(false, true);
	}
	else {
		this->resetView(wasEmpty, true);
	}
	//this->zoomYToData();
}

/**
* Slot that gets the currently selected datasets from the datapanel and plots them as the given type.
* 
* @param type Type of plot to create.
**/
void PlotWidget::plotSelected(PlotType type) {
	this->setPlots(m_DataPanel->getSelectedDatasets(), type);
}

/**
* Function to clear the plot.
**/
void PlotWidget::clear() {
	// Remove plottables
	for (const auto& plottable : m_Plottables)
		delete plottable;
	m_Plottables.clear();

	// Reset axis and labels
	this->xAxis->setLabel("");
	this->yAxis->setLabel("");
	this->xAxis->setRange(0.0, 10.0);
	this->yAxis->setRange(0.0, 10.0);

	this->replot();
}

/**
* Function that generates the right axis labels based on the datasets and type of plot.
**/
void PlotWidget::setAxisLabels() {
	switch (m_Type)
	{
		case PlotType::Time:
		{
			// Create unique list of quantities of datasets
			std::vector<std::string> units;
			for (const auto& plottable : m_Plottables) {
				if (std::find(units.begin(), units.end(), plottable->datasets().front()->unit) == units.end())
					units.push_back(plottable->datasets().front()->unit);
			}

			std::stringstream label;
			if (units.size() < 2) // If only a unique unit is found, we can assume there is also only one unique quantity
				label << m_Plottables.front()->datasets().front()->quantity << " [" << units.front() << "]";
			else {
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

/**
* Function that executes the right-mouse menu when called.
* 
* @param pos Position of the mouse when right-mouse was clicked (passed by signal).
**/
void PlotWidget::showContextMenu(const QPoint& pos) {
	QMenu contextMenu(tr("Context menu"), this);

	QAction acPlotTime("Plot", this);
	connect(&acPlotTime, &QAction::triggered, this, [=]() {plotSelected(PlotType::Time); });
	contextMenu.addAction(&acPlotTime);

	QAction acPlotXY("Plot XY", this);
	connect(&acPlotXY, &QAction::triggered, this, [=]() {plotSelected(PlotType::XY); });
	contextMenu.addAction(&acPlotXY);

	QAction acClear("Clear", this);
	connect(&acClear, SIGNAL(triggered()), this, SLOT(clear()));
	contextMenu.addAction(&acClear);

	QAction acReset("Reset view", this);
	connect(&acReset, &QAction::triggered, [=]() {this->resetView(); });
	contextMenu.addAction(&acReset);

	QAction acResetAll("Reset all views", this);
	connect(&acResetAll, SIGNAL(triggered()), m_PlotManager, SLOT(resetAllViews()));
	contextMenu.addAction(&acResetAll);

	QAction acClip("Clip", this);
	connect(&acClip, SIGNAL(triggered()), this, SLOT(copyToClipboard()));
	contextMenu.addAction(&acClip);

	contextMenu.exec(mapToGlobal(pos));
}

/**
* Function that takes a screenshot of this plot and puts it on the system clipboard (so it can be pasted).
**/
void PlotWidget::copyToClipboard() {
	QApplication::clipboard()->setPixmap(this->grab());
}

/**
* Function that is called when an drag and drop action enters the widget.
* Based on the event data, the drag and drop can either be accepted or rejected.
**/
void PlotWidget::dragEnterEvent(QDragEnterEvent* event) {
	event->acceptProposedAction();
}

/**
* Function that handles what happens when a drag and drop action is released.
**/
void PlotWidget::dropEvent(QDropEvent*) {
	// Datasets can be dragged into plot.
	// If CTRL is held down while the dropping, add them to the existing datasets.
	// Otherwise, replace the datasets by the dragged ones and plot as time series.

	if (QApplication::keyboardModifiers() & Qt::ControlModifier)
		this->addPlots(m_DataPanel->getSelectedDatasets(), m_Type);
	else
		this->setPlots(m_DataPanel->getSelectedDatasets(), PlotType::Time);
}

/**
* When this function is called, the current axis are checked against the data.
* If the axis exceeds the limits defined by the data and padding, modify the axis accordingly.
**/
void PlotWidget::restrictView(const QCPRange& newRange, const QCPRange& oldRange, QCPAxis* axis) {
	bool panning = std::abs(newRange.size() / oldRange.size() - 1) < 1e-5;
	QCPRange limit = (axis == this->xAxis) ? m_RangeLimitX : m_RangeLimitY;
	QCPRange correctedRange = newRange.bounded(limit.lower, limit.upper);
	if (correctedRange != newRange) {
		axis->setRange(correctedRange);
	}
}

/**
* Function that reset the view to fit all data in this plot, including padding.
* By setting the x and y arguments, only specific axis can be reset.
*
* @param x Reset X axis (default = true).
* @param y Reset Y axis (default = true).
**/
void PlotWidget::resetView(bool x, bool y) {
	if (!x && !y) return;
	if (x) this->xAxis->setRange(m_RangeLimitX);
	if (y) this->yAxis->setRange(m_RangeLimitY);
	this->replot();
}

void PlotWidget::zoomYToData() {
	// Find data range in current xAxis range
	QCPRange maxRange;
	m_Plottables.front()->boundedRangeY(this->xAxis->range(), maxRange);
	QCPRange range;
	for (const auto& plottable : m_Plottables) {
		plottable->boundedRangeY(this->xAxis->range(), range);
		maxRange.expand(range);
	}

	// Apply padding and set range
	double padding = maxRange.size() * RANGE_PADDING_Y;
	maxRange.lower -= padding;
	maxRange.upper += padding;
	this->yAxis->setRange(maxRange);
	this->replot();
}

/**
* Function that overrides QWidget and is called when the mouse is moved while it is over this widget.
*
* @param event Event that caused this function to get triggered.
**/
void PlotWidget::mouseMoveEvent(QMouseEvent* event) {
	// Only propagate event is plot is in use (to disable panning and zooming)
	if (!this->isEmpty()) QCustomPlot::mouseMoveEvent(event);

	emit this->mouseMoved(event);
}

/**
* This function overrides QWidget and is called when the scrollwheel is spun while the mouse over this widget.
*
* @param event Event that caused this function to get triggered.
**/
void PlotWidget::wheelEvent(QWheelEvent* event) {
	if (!this->isEmpty())
		QCustomPlot::wheelEvent(event);
	
	emit this->wheelMoved(event);
}

/**
* This function overrides QWidget and is called when the mouse leaves the widget after entering it.
**/
void PlotWidget::leaveEvent(QEvent*) {
	emit this->mouseLeft();
}

/**
* Function that overrides QWidget to catch a double click in the widget
* 
* @param event Event that caused function to get triggered.
**/
void PlotWidget::mouseDoubleClickEvent(QMouseEvent* event) {
	// Time cursor placement
	if (this->isEmpty()) return;
	if (m_Type == PlotType::Time)
	{
		double coordX = this->xAxis->pixelToCoord(event->pos().x());
		double coordY = this->yAxis->pixelToCoord(event->pos().y());
		if (this->xAxis->range().contains(coordX) && this->yAxis->range().contains(coordY))
		{
			coordX = std::round(coordX * 1000.0) / 1000.0; // Round to 3 decimal places
			m_PlotManager->setTimeCursorEnabled(true);
			m_PlotManager->setTimeCursorTime(coordX);
		}
	}

}

/**
* Function that determines the span of all data in the plot and stores it in given ranges.
* 
* @param xRange Range object to store result in for X axis.
* @param yRange Range object to store result in for Y axis.
**/
void PlotWidget::dataRangeSpan(QCPRange& xRange, QCPRange& yRange) {
	if (this->isEmpty()) return;

	xRange = m_Plottables.front()->rangeX();
	yRange = m_Plottables.front()->rangeY();

	for (const auto& plottable : m_Plottables) {
		xRange.lower = std::min({ xRange.lower, plottable->rangeX().lower });
		xRange.upper = std::max({ xRange.upper, plottable->rangeX().upper});
		yRange.lower = std::min({ yRange.lower, plottable->rangeY().lower });
		yRange.upper = std::max({ yRange.upper, plottable->rangeY().upper });
	}
}

/**
* Function to set members that store maximum ranges and paddings.
**/
void PlotWidget::updateRangeAndPadding() {
	QCPRange xSpan, ySpan;
	this->dataRangeSpan(xSpan, ySpan);
	double xPadding = xSpan.size() * RANGE_PADDING_X;
	double yPadding = ySpan.size() * RANGE_PADDING_Y;
	m_RangeLimitX = QCPRange(xSpan.lower - xPadding, xSpan.upper + xPadding);
	m_RangeLimitY = QCPRange(ySpan.lower - yPadding, ySpan.upper + yPadding);
}

/**
* This function returns whether this plot has a time axis that can be aligned.
* Used by the plotManager when aligning time axis of all plots.
**/
bool PlotWidget::timeAxisAlignable() {
	return m_Type == PlotType::Time && !this->isEmpty();
}
