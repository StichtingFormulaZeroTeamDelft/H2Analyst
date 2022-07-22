#include "TimeGraph.h"

/**
* A time graph takes a single datasets and plots it
* with its time vector on the X-axis and its data on the Y-axis.
**/
TimeGraph::TimeGraph(QCustomPlot* parent, const H2A::Dataset* dataset) : AbstractGraph(parent, dataset) {

	auto timeVec = dataset->timeVec();
	auto x = QVector<double>(timeVec.begin(), timeVec.end());
	auto y = QVector<double>(dataset->dataVec.begin(), dataset->dataVec.end());

	m_Graph->setData(x, y);
	m_Graph->setName(QString(dataset->name.c_str()));
	m_Graph->setLineStyle(QCPGraph::lsStepLeft);
}

/**
* Set color of the graph.
* This function sets the graph and other graphical elements.
**/
void TimeGraph::setColor(QColor color) {
	m_Graph->setPen(QPen(color));
}
