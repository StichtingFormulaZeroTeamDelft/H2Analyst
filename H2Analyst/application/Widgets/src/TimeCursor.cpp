#include "TimeCursor.h"

TimeCursor::TimeCursor(AbstractPlot* plot) : QObject(plot),
m_Plot(plot),
m_Enabled(false)
{

	m_Plot->addLayer("cursor", m_Plot->layer("axes"));
	m_Layer = m_Plot->layer("cursor");
	m_Layer->setMode(QCPLayer::lmBuffered);
	m_Layer->setVisible(false);

	m_LineV = new QCPItemLine(m_Plot);
	m_LineV->setLayer(m_Layer);

	connect(m_Plot, SIGNAL(afterReplot()), this, SLOT(draw()));

}

/**
* Draw time cursor on its plot.
**/
void TimeCursor::draw() {
	if (!m_Enabled || m_Plot->isEmpty()) return;

	// Remove current labels
	for (const auto& label : m_Labels)
		m_Plot->removeItem(label);
	m_Labels.clear();

	// Set vertical line
	m_LineV->start->setCoords(m_Time, m_Plot->yAxis->range().lower);
	m_LineV->end->setCoords(m_Time, m_Plot->yAxis->range().upper);

	// Time label
	std::stringstream ss;
	QCPItemText* label = new QCPItemText(m_Plot);
	label->setLayer("cursor");
	label->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
	label->setClipToAxisRect(false);
	label->setBrush(QBrush(Qt::white));
	label->setPen(QPen(Qt::black));
	label->setPadding(QMargins(1, 1, 1, 1));
	label->position->setCoords(m_Time, m_Plot->yAxis->range().lower);
	ss.str("");
	ss << std::fixed << std::setprecision(2) << m_Time;
	label->setText(QString(ss.str().c_str()));
	m_Labels.push_back(label);

	// Data y values
	/*
	QPointF point;
	for (const auto& plottable : m_Plot->plottables())
	{
		point = plottable->dataAt(m_Time);
		
		QCPItemText* label = new QCPItemText(m_Plot);
		label->setLayer("cursor");
		label->setPositionAlignment(Qt::AlignRight | Qt::AlignVCenter);
		label->setClipToAxisRect(false);
		label->setBrush(QBrush(Qt::white));
		label->setPen(QPen(plottable->color()));
		label->setPadding(QMargins(1, 1, 1, 1));
		label->position->setCoords(point);
		QPointF pos = label->position->pixelPosition();
		pos.setX(pos.x() - 1);
		label->position->setPixelPosition(pos);
		ss.str("");
		ss << std::fixed << std::setprecision(2) << point.y();
		label->setText(QString(ss.str().c_str()));
		m_Labels.push_back(label);
	}
	*/

	m_Layer->setVisible(true);
	m_Layer->replot();
}

/**
* Set the time on which the time cursor is placed.
**/
void TimeCursor::setTime(double time) {
	m_Time = time;
	this->draw();
}


/**
* Enable or disable time cursor.
**/
void TimeCursor::setEnabled(bool enable) {
	m_Enabled = enable;
	if (m_Enabled)
	{
		this->draw();
	}
	else
	{
		m_Layer->setVisible(false);
		m_Layer->replot();
	}
}

