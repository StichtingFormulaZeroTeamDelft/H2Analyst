#include "AbstractPlot.h"


AbstractPlot::AbstractPlot(PlotManager* manager) : QCustomPlot(manager),
m_PlotManager(manager),
m_Type(H2A::Abstract),
m_TimeCursor(new TimeCursor(this))
{
	this->xAxis->setTickLabels(false);
	this->yAxis->setTickLabels(false);
}

/**
* Set time of time cursor.
**/
void AbstractPlot::setTimeCursor(double time) {
	m_TimeCursor->setTime(time);
}


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
