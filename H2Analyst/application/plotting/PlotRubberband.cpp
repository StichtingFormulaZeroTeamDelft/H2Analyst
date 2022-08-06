#include "PlotRubberband.h"


Rubberband::Rubberband(AbstractPlot* parent) : QRubberBand(QRubberBand::Rectangle, parent),
m_Plot(parent),
m_Origin(),
m_Active(false)
{
	connect(m_Plot, &AbstractPlot::mouseMoved, this, &Rubberband::mouseMoveEvent);
}

/**
* Call this function to start the rubberband at the supplied origin point.
* 
* @param origin Point to start the rubberband from.
**/
void Rubberband::start(QPoint origin) {
	m_Origin = origin;
	m_Active = true;
	this->setGeometry(QRect(m_Origin, QSize()));
}

/**
* Call this function to end the rubberband and store the resulting ranges in the supplied objects.
* 
* @param xRange Object to store the X range in.
* @param yRange Object to store the Y range in.
**/
void Rubberband::end() {
	this->hide();
	m_Active = false;

	QCPRange xRange(	m_Plot->xAxis->pixelToCoord(this->geometry().left()),
						m_Plot->xAxis->pixelToCoord(this->geometry().right()));
	QCPRange yRange(	m_Plot->yAxis->pixelToCoord(this->geometry().bottom()),
						m_Plot->yAxis->pixelToCoord(this->geometry().top()));
	
	m_Plot->xAxis->setRange(xRange);
	m_Plot->yAxis->setRange(yRange);
	m_Plot->replot();
}

/**
* Call this function to cancel the rubberband.
**/
void Rubberband::cancel() {
	this->hide();
	m_Active = false;
}


/**
* This slot should be connected to a signal that is emitted when the mouse is moved.
* 
* @param event Event that caused the signal to be triggered (supplied by signal).
**/
void Rubberband::mouseMoveEvent(QMouseEvent* event) {
	if (!m_Active) return;

	m_Mouse = event->pos();
	QRect rect = QRect(m_Origin, m_Mouse).normalized();

	// If rect is too small to make sense, don't continue
	if (rect.width() < 5 && rect.height() < 5) {
		this->hide();
		return;
	}
	this->show();


	// If SHIFT is pressed, check if aspect ratio of shape is such that rubberband should snap to horizontal or vertical
	if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
		m_SnappedH = rect.height() < rect.width();

		if (m_SnappedH) {
			rect.setTop(m_Plot->axisRect()->top());
			rect.setBottom(m_Plot->axisRect()->bottom());
		}
		else {
			rect.setLeft(m_Plot->axisRect()->left());
			rect.setRight(m_Plot->axisRect()->right());
		}
	}

	// Set shape
	this->setGeometry(rect);
}




