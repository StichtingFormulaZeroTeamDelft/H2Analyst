#include "Plottable.h"


Plottable::Plottable(QCustomPlot* plot) : QObject(plot),
m_Plot(plot)
{

}

const std::vector<const H2A::Dataset*> Plottable::datasets()
{
	return std::vector<const H2A::Dataset*>();
}

Plottable::~Plottable()
{
	// Purely virtual
}