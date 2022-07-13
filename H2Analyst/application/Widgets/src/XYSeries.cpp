#include "XYSeries.h"


XYSeries::XYSeries(QCustomPlot* plot, const std::vector<const H2A::Dataset*> datasets) : Plottable(plot)
{

	// Check dataset vector contains 2 datasets
	if (datasets.size() != 2) {
		std::stringstream ss;
		ss << datasets.size() << " datasets selected. Please select 2 for XY plot.";
		H2A::Dialog::message(QString(ss.str().c_str()));
	}

	m_Datasets = datasets;

	// Create QVectors from data
	std::vector<double> time;
	std::vector<std::vector<double>> data;
	H2A::resample(datasets, time, data);
	QVector<double> x(data[0].begin(), data[0].end());
	QVector<double> y(data[1].begin(), data[1].end());


	// Save data range
	m_RangeX = QCPRange(*std::min_element(x.begin(), x.end()), *std::max_element(x.begin(), x.end()));
	m_RangeY = QCPRange(*std::min_element(y.begin(), y.end()), *std::max_element(y.begin(), y.end()));

	plot->setCurrentLayer("main");
	m_Curve = new QCPCurve(m_Plot->xAxis, m_Plot->yAxis);
	m_Curve->setAntialiased(true);
	m_Curve->setData(x, y);

	this->setAxisLabels();

}

void XYSeries::setColor(QColor color) {
	m_Color = color;
	m_Curve->setPen(QPen(m_Color));
}

void XYSeries::setAxisLabels() {
	std::stringstream ss;
	ss << m_Datasets[0]->name << " (" << m_Datasets[0]->quantity << " [" << m_Datasets[0]->unit << "])";
	m_Plot->xAxis->setLabel(QString(ss.str().c_str()));
	
	ss.str("");
	ss << m_Datasets[1]->name << " (" << m_Datasets[1]->quantity << " [" << m_Datasets[1]->unit << "])";
	m_Plot->yAxis->setLabel(QString(ss.str().c_str()));
}

XYSeries::~XYSeries() {
	if (m_Plot->graphCount() > 0) // This check prevents warnings when program is closed
	{
		//m_Plot->removeGraph(m_Graph);
	}
}

const QPointF XYSeries::dataAt(double time) const {
	// TODO: implementation
	return QPointF(0.0, 0.0);
}
