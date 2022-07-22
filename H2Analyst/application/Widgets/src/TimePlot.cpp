#include "TimePlot.h"

/**
* Standard constructor.
**/
TimePlot::TimePlot(QWidget* parent) : AbstractPlot(parent)
{
	m_Type = H2A::Time;
	this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iMultiSelect);
	this->setAxisLabels();
	this->legend->setVisible(true);
}

/**
* Plot the given list of datasets.
* 
* @param datasets Datasets to plot.
* @param clearFirst Flag if plot should be cleared before plotting datasets.
**/
void TimePlot::plot(std::vector<const H2A::Dataset*> datasets, bool clearFirst) {
	if (datasets.size() == 0) return;

	if (clearFirst) this->clear();

	for (const auto& dataset : datasets) {

		// Check if dataset is not already plotted
		bool alreadyPlotted = false;
		for (const auto& graph : m_Graphs) {
			auto datasets = graph->datasets();
			if (std::find(datasets.begin(), datasets.end(), dataset) != datasets.end()) {
				alreadyPlotted = true;
				break;
			}
		}
		if (alreadyPlotted) continue;

		TimeGraph* graph = new TimeGraph(this, dataset);
		graph->setColor(H2A::PlotColors[m_Graphs.size() % H2A::PlotColors.size()]);
		m_Graphs.push_back(graph);
	}

	this->setAxisLabels();

	// Set hard view limits based on graph ranges
	m_LimHardX = m_Graphs.front()->rangeX();
	m_LimHardY = m_Graphs.front()->rangeY();
	for (auto const& graph : m_Graphs) {
		m_LimHardX.expand(graph->rangeX());
		m_LimHardY.expand(graph->rangeY());
	}
	m_LimHardX = QCPRange(m_LimHardX.lower - LIMIT_PADDING * m_LimHardX.size(), m_LimHardX.upper + LIMIT_PADDING * m_LimHardX.size());
	m_LimHardY = QCPRange(m_LimHardY.lower - LIMIT_PADDING * m_LimHardY.size(), m_LimHardY.upper + LIMIT_PADDING * m_LimHardY.size());

	this->resetView();
}

/**
* Set axis labels of the plot.
**/
void TimePlot::setAxisLabels() {
	this->xAxis->setLabel("Time [sec]");
	this->xAxis->setTickLabels(false);
	this->yAxis->setTickLabels(false);
	this->yAxis->setLabel("");

	if (this->isEmpty()) return;

	// Create set of dataset units
	std::set<std::string> units;
	auto datasets = this->datasets();
	for (const auto& dataset : datasets) units.insert(dataset->unit);

	std::stringstream ss;
	if (units.size() == 1) {
		ss << datasets.front()->quantity << "[" << datasets.front()->unit << "]";
		this->yAxis->setLabel(QString(ss.str().c_str()));
	}
	else {
		ss << "Mixed quantities [";
		for (const auto& dataset : datasets) {
			ss << dataset->unit;
			if (dataset != datasets.back()) ss << ",";
		}
		ss << "]";
		this->yAxis->setLabel(QString(ss.str().c_str()));
	}

	this->xAxis->setTickLabels(true);
	this->yAxis->setTickLabels(true);
}

/**
* Reset view to fit all graphs with some padding.
**/
void TimePlot::resetView() {
	QCPRange rangeX = this->dataRangeX();
	double padding = rangeX.size() * STD_VIEW_PADDING;
	rangeX = QCPRange(rangeX.lower - padding, rangeX.upper + padding);
	
	QCPRange rangeY = this->dataRangeY();
	padding = rangeY.size() * STD_VIEW_PADDING;
	rangeY = QCPRange(rangeY.lower - padding, rangeY.upper + padding);

	this->xAxis->setRange(rangeX);
	this->yAxis->setRange(rangeY);
	this->replot();
}


