#include "PlotCrosshairs.h"

Crosshairs::Crosshairs(QCustomPlot* parentPlot, QString layerName, QCPLayer *layerBelow) : QObject(parentPlot),
m_Parent(parentPlot)
{
	m_Parent->addLayer(layerName, layerBelow);
	m_Layer = m_Parent->layer(layerName);

	m_LineH = new QCPItemLine(m_Parent);
	m_LineH->setLayer(m_Layer);
	m_LineV = new QCPItemLine(m_Parent);
	m_LineV->setLayer(m_Layer);

	m_LabelX = new QCPItemText(m_Parent);
	m_LabelX->setLayer(m_Layer);
	m_LabelX->setClipToAxisRect(false);
	m_LabelX->setBrush(QBrush(Qt::white));
	m_LabelX->setPen(QPen(Qt::black));
	m_LabelX->setPadding(QMargins(2, 2, 2, 2));
	m_LabelX->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
	m_LabelX->position->setType(QCPItemPosition::ptPlotCoords);

	m_LabelY = new QCPItemText(m_Parent);
	m_LabelY->setLayer(m_Layer);
	m_LabelY->setClipToAxisRect(false);
	m_LabelY->setBrush(QBrush(Qt::white));
	m_LabelY->setPen(QPen(Qt::black));
	m_LabelY->setPadding(QMargins(2, 2, 2, 2));
	m_LabelY->setPositionAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	m_LabelY->position->setType(QCPItemPosition::ptPlotCoords);

	this->disable();
}


void Crosshairs::update(QMouseEvent *event)
{
	// Coordinate calculation
	m_Pos.setX(m_Parent->xAxis->pixelToCoord(event->pos().x()));
	m_Pos.setY(m_Parent->yAxis->pixelToCoord(event->pos().y()));	
	this->update();
}

void Crosshairs::update()
{
	if (!m_Enabled) return;

	// Crosshair lines
	m_LineH->start->setCoords(m_Parent->xAxis->range().lower, m_Pos.y());
	m_LineH->end->setCoords(m_Parent->xAxis->range().upper, m_Pos.y());
	m_LineV->start->setCoords(m_Pos.x(), m_Parent->yAxis->range().upper);
	m_LineV->end->setCoords(m_Pos.x(), m_Parent->yAxis->range().lower);

	// Labels
	m_LabelX->position->setCoords(m_Pos.x(), m_Parent->yAxis->range().lower);
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << m_Pos.x();
	m_LabelX->setText(QString(stream.str().c_str()));

	m_LabelY->position->setCoords(m_Parent->xAxis->range().lower, m_Pos.y());
	stream.str(""); // Clear stream
	stream << std::fixed << std::setprecision(2) << m_Pos.y();
	m_LabelY->setText(QString(stream.str().c_str()));

	m_Layer->replot();
}

void Crosshairs::enable()
{
	m_Enabled = true;
	m_Layer->setVisible(true);
}

void Crosshairs::disable()
{
	m_Enabled = false;
	m_Layer->setVisible(false);
	m_Layer->replot(); // needed to clear crosshairs from plot
}