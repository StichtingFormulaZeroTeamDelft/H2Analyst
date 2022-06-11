#pragma once

#include <vector>

#include <QObject>

#include "qcustomplot.h"
#include "DataStructures.h"

class Plottable : public QObject
{
	Q_OBJECT


protected:
	QCustomPlot* m_Plot;
	QCPRange m_RangeX;
	QCPRange m_RangeY;

public:

	Plottable(QCustomPlot* plot);
	virtual ~Plottable();

	QCPRange rangeX() { return m_RangeX; };
	QCPRange rangeY() { return m_RangeY; };
	virtual const std::vector<const H2A::Dataset*> datasets();

};

