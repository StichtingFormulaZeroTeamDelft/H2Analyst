#pragma once

#include "AbstractGraph.h"

class TimeGraph : public AbstractGraph
{

public:
	TimeGraph(QCustomPlot* parent, const H2A::Dataset* dataset);

	void setColor(QColor color) override;
	bool dataAt(double time, QPointF& point) const override;
};

