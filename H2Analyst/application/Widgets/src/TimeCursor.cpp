#include "TimeCursor.h"

TimeCursor::TimeCursor(AbstractPlot* plot) : QWidget(plot),
m_Plot(plot),
m_Layer(nullptr),
m_Time(0.0),
m_Enabled(false),
m_LineV(nullptr),
m_Labels(),
m_Dragging(false)
{

	m_Plot->addLayer("cursor", m_Plot->layer("axes"));
	m_Layer = m_Plot->layer("cursor");
	m_Layer->setMode(QCPLayer::lmBuffered);
	m_Layer->setVisible(false);

	this->setMouseTracking(true);

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
	QPointF point;
	for (const auto& graph : m_Plot->graphs())
	{
		QPointF point;
		bool found = graph->dataAt(m_Time, point);
		if (!found) continue;
		
		QCPItemText* label = new QCPItemText(m_Plot);
		label->setLayer("cursor");
		label->setPositionAlignment(Qt::AlignRight | Qt::AlignVCenter);
		label->setClipToAxisRect(false);
		label->setBrush(QBrush(Qt::white));
		label->setPen(QPen(graph->color()));
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

	// Update hitbox
	QRect hitbox;
	hitbox.setBottom(m_Plot->yAxis->coordToPixel(m_Plot->yAxis->range().lower));
	hitbox.setTop(m_Plot->yAxis->coordToPixel(m_Plot->yAxis->range().upper));
	hitbox.setLeft(m_Plot->xAxis->coordToPixel(m_Time) - HITBOX_RANGE);
	hitbox.setRight(m_Plot->xAxis->coordToPixel(m_Time) + HITBOX_RANGE);
	this->setGeometry(hitbox);

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


void TimeCursor::mouseMoveEvent(QMouseEvent* event) {
	if (!m_Enabled) return;
	this->setCursor(Qt::SizeHorCursor);

	if (m_Dragging) {
		this->setTime(m_Plot->xAxis->pixelToCoord(m_Plot->mapFromGlobal(QCursor::pos()).x()));
		emit m_Plot->timeCursorPlaced(m_Time);
	}
}

void TimeCursor::mousePressEvent(QMouseEvent* event) {
	if (!m_Enabled) return;
	m_Dragging = true;
}

void TimeCursor::mouseReleaseEvent(QMouseEvent* event) {
	m_Dragging = false;
}

/**
* Enable or disable time cursor.
**/
void TimeCursor::setEnabled(bool enable) {
	m_Enabled = enable;
	if (m_Enabled) {
		this->draw();
	}
	else {
		m_Layer->setVisible(false);
		m_Layer->replot();
	}
}
