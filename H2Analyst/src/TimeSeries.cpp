#include "TimeSeries.h"


TimeSeries::TimeSeries(QCustomPlot* plot, const H2A::Dataset* dataset) : Plottable(plot)
{
	m_Datasets = std::vector<const H2A::Dataset*>({dataset});
	m_Dataset = dataset;

	// Create QVectors from dataset data
	QVector<double> x(m_Dataset->timeVec.begin(), m_Dataset->timeVec.end());
	QVector<double> y(m_Dataset->dataVec.begin(), m_Dataset->dataVec.end());

	// Save data range
	m_RangeX = QCPRange(*std::min_element(x.begin(), x.end()), *std::max_element(x.begin(), x.end()));
	m_RangeY = QCPRange(*std::min_element(y.begin(), y.end()), *std::max_element(y.begin(), y.end()));

	// Add graph and data
	m_Graph = m_Plot->addGraph();
	m_Graph->setLineStyle(QCPGraph::lsStepLeft);
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
	m_Graph->setPen(QPen(color));
	m_Label->setPen(QPen(color));
}

// Destructor to make sure plottables are removed from the graph
TimeSeries::~TimeSeries()
{
	if (m_Plot->graphCount() > 0) // This check prevents warnings when program is closed
	{
		m_Plot->removeGraph(m_Graph);
		m_Plot->removeItem(m_Label);
	}
}