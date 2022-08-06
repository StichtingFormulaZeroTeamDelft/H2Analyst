#include "Plottable.h"


Plottable::Plottable(QCustomPlot* plot) : QObject(plot),
m_Plot(plot)
{
}

Plottable::~Plottable()
{
	// Purely virtual
}

void Plottable::setColor(QColor color) {
	m_Color = color;
}

const QPointF Plottable::dataAt(double time) const {
	return QPointF(0.0, 0.0);
}

