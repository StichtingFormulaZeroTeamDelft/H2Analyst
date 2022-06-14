#include "Plottable.h"


Plottable::Plottable(QCustomPlot* plot) : QObject(plot),
m_Plot(plot)
{

}

Plottable::~Plottable()
{
	// Purely virtual
}