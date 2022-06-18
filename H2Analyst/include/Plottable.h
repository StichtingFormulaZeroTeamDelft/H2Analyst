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
	QColor m_Color;
	std::vector<const H2A::Dataset*> m_Datasets;

public:

	Plottable(QCustomPlot* plot);
	virtual ~Plottable();

	virtual void setColor(QColor color);

	const QCPRange rangeX() const { return m_RangeX; };
	const QCPRange rangeY() const { return m_RangeY; };
	virtual const bool boundedRangeY(const QCPRange bounds, QCPRange& range) const { return false; };
	virtual const QPointF dataAt(double time) const;
	virtual const QColor color() const { return m_Color; };
	const std::vector<const H2A::Dataset*> datasets() { return m_Datasets; };

};

