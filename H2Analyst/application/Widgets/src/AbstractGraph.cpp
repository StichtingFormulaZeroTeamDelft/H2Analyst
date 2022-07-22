#include "AbstractGraph.h"

AbstractGraph::AbstractGraph(QCustomPlot* parent, const H2A::Dataset* dataset) :
m_Parent(parent),
m_Graph(nullptr),
m_Datasets() {

	m_Graph = m_Parent->addGraph();
	m_Datasets.push_back(dataset);
}

/**
* Returns the range of the X data.
**/
QCPRange AbstractGraph::rangeX() const {
	bool rangeFound = false;
	auto range = m_Graph->getKeyRange(rangeFound);
	return range;
}

/**
* Returns the range of the Y data.
**/
QCPRange AbstractGraph::rangeY() const {
	bool rangeFound = false;
	auto range = m_Graph->getValueRange(rangeFound);
	return range;
}

/**
* Destructor.
**/
AbstractGraph::~AbstractGraph() {
	m_Parent->removeGraph(m_Graph);
}

