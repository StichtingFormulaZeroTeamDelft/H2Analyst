#pragma once

#include "qcustomplot.h"
#include "DataStructures.h"

class AbstractGraph
{

protected:
	QCustomPlot* m_Parent;
	std::vector<const H2A::Dataset*> m_Datasets;
	QCPGraph* m_Graph;

public:

	AbstractGraph(QCustomPlot* parent, const H2A::Dataset* dataset);
	~AbstractGraph();

	QCPGraph* graph() { return m_Graph; };
	std::vector<const H2A::Dataset*> datasets() const { return m_Datasets; };
	virtual QCPRange rangeX() const;
	virtual QCPRange rangeY() const;
	virtual void setColor(QColor color) {};

};


