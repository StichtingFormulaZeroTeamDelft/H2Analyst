#include "AbstractPlot.h"


AbstractPlot::AbstractPlot(QWidget* parent) : QCustomPlot(parent),
m_Type(H2A::Abstract),
m_TimeCursor(new TimeCursor(this)),
m_Rubberband(new Rubberband(this)),
m_BusyEnforcingViewLimits(false)
{
	this->xAxis->setTickLabels(false);
	this->yAxis->setTickLabels(false);

	m_LimHardX = this->xAxis->range();
	m_LimHardY = this->yAxis->range();
	connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(enforceViewLimits()));
	connect(this->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(enforceViewLimits()));

	this->setAcceptDrops(true);
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(contextMenu(const QPoint&)));
}

/**
* Returns a list of datasets that are used in the graphs of this plot.
**/
std::vector<const H2A::Dataset*> AbstractPlot::datasets() const
{
	std::vector<const H2A::Dataset*> datasets;
	for (const auto& graph : m_Graphs) {
		auto graphDatasets = graph->datasets();
		datasets.insert(datasets.end(), graphDatasets.begin(), graphDatasets.end());
	}
	return datasets;
}

/**
* Returns the range that spans all X data in the graphs.
**/
QCPRange AbstractPlot::dataRangeX() const {
	if (this->isEmpty()) return QCPRange();

	QCPRange range = m_Graphs.front()->rangeX();
	for (const auto& graph : m_Graphs)
		range.expand(graph->rangeX());

	return range;
}

/**
* Returns the range that spans all Y data in the graphs.
**/
QCPRange AbstractPlot::dataRangeY() const {
	if (this->isEmpty()) return QCPRange();

	QCPRange range = m_Graphs.front()->rangeY();
	for (const auto& graph : m_Graphs)
		range.expand(graph->rangeY());

	return range;
}

/**
* Create graphs with the given datasets.
**/
void AbstractPlot::plot(std::vector<const H2A::Dataset*> datasets, bool clearFirst) {
	if (clearFirst) m_Graphs.clear();
	for (const auto& dataset : datasets) {
		AbstractGraph* graph = new AbstractGraph(this, dataset);
		m_Graphs.push_back(graph);
	}
}

/**
* Clear plot of all graphs.
**/
void AbstractPlot::clear() {
	for (const auto& graph : m_Graphs) {
		delete graph;
	}
	m_Graphs.clear();
	this->legend->setVisible(false);
	this->resetView();
	this->replot();
}

/**
* Slots to set time cursor enable and time.
**/
void AbstractPlot::setTimeCursorEnabled(bool enable) { m_TimeCursor->setEnabled(enable); }
void AbstractPlot::setTimeCursorTime(double time) { m_TimeCursor->setTime(time); }

/**
* Function that overrides Base and is called when the mouse is moved while it is over this widget.
*
* @param event Event that caused this function to get triggered.
**/
void AbstractPlot::mouseMoveEvent(QMouseEvent* event) {
	// Only propagate event is plot is in use (to disable panning and zooming)
	if (!this->isEmpty())
		QCustomPlot::mouseMoveEvent(event);
	emit this->mouseMoved(event);
}

/**
* This function overrides Base and is called when the mouse leaves the widget after entering it.
**/
void AbstractPlot::leaveEvent(QEvent*) {
	emit this->mouseLeft();
}

/**
* Override function for mouse presses.
*
* @param event Event that caused this function to get triggered.
**/
void AbstractPlot::mousePressEvent(QMouseEvent* event) {
	
	// Ignore all clicks if plot is still empty
	if (this->isEmpty()) return;

	// Rubberband
	if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
		m_Rubberband->start(event->pos());
	}
	else {
		QCustomPlot::mousePressEvent(event);
	}
}

/**
* Override function for double mouse presses.
*
* @param event Event that caused this function to get triggered.
**/
void AbstractPlot::mouseDoubleClickEvent(QMouseEvent* event) {
	if (this->isEmpty()) return;

	// Time cursor
	emit this->timeCursorPlaced(this->xAxis->pixelToCoord(event->pos().x()));
	
	QCustomPlot::mouseDoubleClickEvent(event);
}

/**
* Override function for mouse releases.
*
* @param event Event that caused this function to get triggered.
**/
void AbstractPlot::mouseReleaseEvent(QMouseEvent* event) {

	if (m_Rubberband->isActive()) m_Rubberband->end();

	QCustomPlot::mouseReleaseEvent(event);
}





/**
* This function overrides QWidget and is called when the scrollwheel is spun while the mouse over this widget.
*
* @param event Event that caused this function to get triggered.
**/
void AbstractPlot::wheelEvent(QWheelEvent* event) {
	if (!this->isEmpty()) {
		QCustomPlot::wheelEvent(event);
		this->replot();
	}
}

/**
* Function that is called when an drag and drop action enters the widget.
* Based on the event data, the drag and drop can either be accepted or rejected.
**/
void AbstractPlot::dragEnterEvent(QDragEnterEvent* event) {
	event->acceptProposedAction(); // Accept any drop
}

/**
* Function that handles what happens when a drag and drop action is dropped on this plot.
**/
void AbstractPlot::dropEvent(QDropEvent*) {
	// If data is dropped on an AbstractPlot object, it should be changed to a TimePlot and plot the data.
	bool ctrlPressed = (QApplication::keyboardModifiers() & Qt::ControlModifier);
	emit this->plotSelected(this, H2A::Time, !ctrlPressed);
}

/**
* Function that makes sure the X axis does not past its limits.
**/
void AbstractPlot::enforceViewLimits() {
	if (m_BusyEnforcingViewLimits) return;

	m_BusyEnforcingViewLimits = true;
	this->xAxis->setRange(this->xAxis->range().bounded(m_LimHardX.lower, m_LimHardX.upper));
	this->yAxis->setRange(this->yAxis->range().bounded(m_LimHardY.lower, m_LimHardY.upper));
	m_BusyEnforcingViewLimits = false;
}

/**
* Function that creates the abstract context menu that applies to all plots.
**/
void AbstractPlot::createAbstractContextMenu(QMenu& menu) {

	QAction* acPlot = new QAction("Plot");
	acPlot->setEnabled(false);
	menu.addAction(acPlot);

	QMenu* plotMenu = menu.addMenu(QIcon(QPixmap(":/icons/more-information")), "Other plots");

	QAction* acPlotXY = new QAction("XY");
	acPlotXY->setEnabled(false);
	plotMenu->addAction(acPlotXY);

	QAction* acResetView = new QAction(QIcon(QPixmap(":/icons/uno")), QString("Reset view"));
	connect(acResetView, SIGNAL(triggered(bool)), this, SLOT(resetView()));
	menu.addAction(acResetView);

	QAction* acClear = new QAction(QString("Clear"));
	connect(acClear, SIGNAL(triggered(bool)), this, SLOT(clear()));
	menu.addAction(acClear);

	QAction* acDelete = new QAction(QIcon(QPixmap(":/icons/remove")), QString("Delete plot"));
	connect(acDelete, &QAction::triggered, [=]() {emit this->deleteMe(this); });
	menu.addAction(acDelete);
}

/**
* Slot that opens context (right-click) menu.
**/
void AbstractPlot::contextMenu(const QPoint& pos) {
	QMenu menu;
	this->createAbstractContextMenu(menu);
	menu.exec(mapToGlobal(pos));
}

/**
* Function that is used to add extra functionality to standard replot.
**/
void AbstractPlot::replot() {
	QCustomPlot::replot();
	m_TimeCursor->draw();
}

