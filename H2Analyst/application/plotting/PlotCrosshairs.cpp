#include "PlotCrosshairs.h"

Crosshairs::Crosshairs(AbstractPlot* parent) : QObject(parent),
m_Parent(parent),
m_Pos(0.0, 0.0)
{

	m_Parent->addLayer("crosshairs", m_Parent->layer("axes"));
	m_Layer = m_Parent->layer("crosshairs");
	m_Layer->setMode(QCPLayer::lmBuffered);

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

	connect(m_Parent, SIGNAL(mouseMoved(QMouseEvent*)), this, SLOT(update(QMouseEvent*)));
	connect(m_Parent, SIGNAL(wheelMoved(QWheelEvent*)), this, SLOT(update(QWheelEvent*)));
	connect(m_Parent, &AbstractPlot::mouseLeft, [=]() { this->hide(); });

	this->disable();
}

void Crosshairs::update()
{
	if (!m_Enabled) return;

	// Check if cursor is within plot area of widget
	if (m_Parent->xAxis->range().lower < m_Pos.x() &&
		m_Pos.x() < m_Parent->xAxis->range().upper &&
		m_Parent->yAxis->range().lower < m_Pos.y() &&
		m_Pos.y() < m_Parent->yAxis->range().upper)
	{
		m_Layer->setVisible(true);
		m_Parent->setCursor(Qt::BlankCursor);

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
	else
	{
		this->hide();
	}
}


void Crosshairs::update(QMouseEvent *event)
{
	// Coordinate calculation
	m_Pos.setX(m_Parent->xAxis->pixelToCoord(event->pos().x()));
	m_Pos.setY(m_Parent->yAxis->pixelToCoord(event->pos().y()));
	this->update();
}

void Crosshairs::update(QWheelEvent* event)
{
	// Coordinate calculation
	m_Pos.setX(m_Parent->xAxis->pixelToCoord(event->position().x()));
	m_Pos.setY(m_Parent->yAxis->pixelToCoord(event->position().y()));
	this->update();
}



void Crosshairs::enable()
{
	m_Enabled = true;
	m_Layer->setVisible(true);
	this->update();
}

void Crosshairs::hide()
{
	m_Layer->setVisible(false);
	m_Parent->setCursor(Qt::ArrowCursor);
	m_Layer->replot();
}

void Crosshairs::disable()
{
	m_Enabled = false;
	this->hide();
}