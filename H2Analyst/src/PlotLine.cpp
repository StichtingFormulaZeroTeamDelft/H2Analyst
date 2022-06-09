#include "PlotLine.h"

PlotLine::PlotLine(QCustomPlot* parentPlot, const H2A::Dataset* dataset) : QObject(parentPlot),
m_Parent(parentPlot),
m_Dataset(dataset),
m_Graph(nullptr),
m_Label(nullptr),
m_Color(Qt::black)
{
	// Create QVectors from dataset data
	QVector<double> x(m_Dataset->timeVec.begin(), m_Dataset->timeVec.end());
	QVector<double> y(m_Dataset->dataVec.begin(), m_Dataset->dataVec.end());

	// Save data range
	rangeX = QCPRange(x.front(), x.back()); // Assumes time vector always points 'to the right'
	rangeY = QCPRange(*std::min_element(y.begin(), y.end()), *std::max_element(y.begin(), y.end()));

	// Add graph and data
	m_Graph = m_Parent->addGraph();
	m_Graph->setLineStyle(QCPGraph::lsStepLeft);
	m_Graph->setData(x, y);

	// Create label (legend)
	m_Label = new QCPItemText(m_Parent);
	m_Label->setLayer("legend");
	m_Label->setClipToAxisRect(true);
	m_Label->setBrush(QBrush(Qt::white));
	m_Label->setPadding(QMargins(2, 2, 2, 2));
	m_Label->setPositionAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_Label->position->setType(QCPItemPosition::ptPlotCoords);
	m_Label->setText(QString(m_Dataset->name.c_str()));
	this->updateLabelPosition();
	connect(m_Parent->xAxis, SIGNAL(rangeChanged(const QCPRange&)), this, SLOT(updateLabelPosition()));
	connect(m_Parent->yAxis, SIGNAL(rangeChanged(const QCPRange&)), this, SLOT(updateLabelPosition()));
}

void PlotLine::setColor(QColor color)
{
	m_Color = color;

	// Set pen for graph
	m_Graph->setPen(QPen(m_Color));
	QPen pen = m_Graph->pen();
	// Change pen to change appearance of selected plot
	m_Graph->selectionDecorator()->setPen(pen);

	// Pen for label
	m_Label->setPen(QPen(m_Color));
}


void PlotLine::updateLabelPosition()
{
	std::string name = m_Dataset->name;
	auto data = m_Graph->data();
	auto begin = data->findBegin(m_Parent->xAxis->range().upper);
	m_Label->position->setCoords(m_Parent->xAxis->range().upper, begin->value);
	QPointF pos = m_Label->position->pixelPosition();
	pos.setX(pos.x() - 1);
	m_Label->position->setPixelPosition(pos);
}

PlotLine::~PlotLine()
{
	m_Parent->removePlottable(m_Graph);
	m_Parent->removeItem(m_Label);
}


