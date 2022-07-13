#include "TimeSeries.h"


TimeSeries::TimeSeries(QCustomPlot* plot, const H2A::Dataset* dataset) : Plottable(plot)
{
	m_Datasets = std::vector<const H2A::Dataset*>({dataset});
	m_Dataset = dataset;

	// Create QVectors from dataset data
	std::vector<double> time = m_Dataset->timeVec();
	QVector<double> x(time.begin(), time.end());
	QVector<double> y(m_Dataset->dataVec.begin(), m_Dataset->dataVec.end());

	// Save data range
	m_RangeX = QCPRange(*std::min_element(x.begin(), x.end()), *std::max_element(x.begin(), x.end()));
	m_RangeY = QCPRange(*std::min_element(y.begin(), y.end()), *std::max_element(y.begin(), y.end()));

	// Add graph and data
	m_Graph = m_Plot->addGraph();
	m_Graph->setLineStyle(QCPGraph::lsStepLeft);
	m_Graph->setAntialiased(true);
	m_Graph->setData(x, y);

	// Create label (legend)
	m_Label = new QCPItemText(m_Plot);
	m_Label->setLayer("legend");
	m_Label->setClipToAxisRect(true);
	m_Label->setBrush(QBrush(Qt::white));
	m_Label->setPadding(QMargins(2, 2, 2, 2));
	m_Label->setPositionAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_Label->position->setType(QCPItemPosition::ptPlotCoords);
	m_Label->setText(QString(m_Dataset->name.c_str()));
	this->updateLabelPosition();
	connect(m_Plot->xAxis, SIGNAL(rangeChanged(const QCPRange&)), this, SLOT(updateLabelPosition()));
	connect(m_Plot->yAxis, SIGNAL(rangeChanged(const QCPRange&)), this, SLOT(updateLabelPosition()));

}

// Function that places the legend label at the correct spot, namely
// touching the right edge at the heigt where the line crosses the edge.
void TimeSeries::updateLabelPosition()
{
	std::string name = m_Dataset->name;
	auto data = m_Graph->data();
	auto begin = data->findBegin(m_Plot->xAxis->range().upper);
	m_Label->position->setCoords(m_Plot->xAxis->range().upper, begin->value);
	QPointF pos = m_Label->position->pixelPosition();
	pos.setX(pos.x() - 1);
	m_Label->position->setPixelPosition(pos);
}

// Color setting
void TimeSeries::setColor(QColor color)
{
	m_Color = color;
	m_Graph->setPen(QPen(m_Color));
	m_Label->setPen(QPen(m_Color));
}

// Destructor to make sure plottables are removed from the graph
TimeSeries::~TimeSeries() {
	if (m_Plot->graphCount() > 0) // This check prevents warnings when program is closed
	{
		m_Plot->removeGraph(m_Graph);
		m_Plot->removeItem(m_Label);
	}
}

/**
* Function that returns the intersection point of this plot at a given time.
*
* @param time Time to intersect plot at.
**/
const QPointF TimeSeries::dataAt(double time) const {
	std::vector<double> timeVec = m_Dataset->timeVec();
	if (time < timeVec.front())
		return QPointF(0.0, 0.0);

	for (size_t cursor = 1; cursor < timeVec.size(); ++cursor) {
		if (timeVec[cursor] > time)
			return QPointF(time, m_Dataset->dataVec[cursor - 1]);
	}

	return QPointF(0.0, 0.0);
}

const bool TimeSeries::boundedRangeY(const QCPRange bounds, QCPRange& range) const
{
	bool foundRange;
	range = m_Graph->getValueRange(foundRange, QCP::sdBoth, bounds);
	return foundRange;
}
