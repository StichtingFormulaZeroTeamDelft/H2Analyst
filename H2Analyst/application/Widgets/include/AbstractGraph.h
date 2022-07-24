#pragma once

#include "qcustomplot.h"
#include "DataStructures.h"

class AbstractGraph
{

protected:
	QCustomPlot* m_Parent;
	QColor m_Color;
	std::vector<const H2A::Dataset*> m_Datasets;
	QCPGraph* m_Graph;

public:

	AbstractGraph(QCustomPlot* parent, const H2A::Dataset* dataset);
	~AbstractGraph();

	QCPGraph* graph() { return m_Graph; };
	std::vector<const H2A::Dataset*> datasets() const { return m_Datasets; };
	virtual QCPRange rangeX() const;
	virtual QCPRange rangeY() const;
	virtual bool dataAt(double time, QPointF& point) const { return false; };
	virtual void setColor(QColor color) {};
	virtual QColor color() const { return m_Color; }
};


