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
	std::vector<const H2A::Dataset*> m_Datasets;

public:

	Plottable(QCustomPlot* plot);
	virtual ~Plottable();

	QCPRange rangeX() { return m_RangeX; };
	QCPRange rangeY() { return m_RangeY; };
	const std::vector<const H2A::Dataset*> datasets() { return m_Datasets; };

};

