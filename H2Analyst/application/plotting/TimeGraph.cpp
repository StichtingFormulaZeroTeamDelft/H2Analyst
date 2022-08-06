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
	m_Color = color;
	m_Graph->setPen(QPen(color));
}

/**
* Function that find the coordinate that belongs to a given time. Returns true if time is within data range and a point is found.
* 
* @param time Time to get coordinate from.
* @param point Point to store result into.
**/
bool TimeGraph::dataAt(double time, QPointF& point) const {
	auto data = m_Graph->data();
	bool rangeFound = true;
	auto keyRange = data->keyRange(rangeFound);
	if (time < keyRange.lower || time > keyRange.upper) return false;

	for (auto i = 0; i < data->size(); ++i) {
		if (data->at(i)->mainKey() > time) {
			point = QPointF(data->at(i)->mainKey(), data->at(i)->mainValue());
			return true;
		}
	}
	return false;
}