#pragma once

#include "Plottable.h"
#include "DataStructures.h"


class TimeSeries : public Plottable
{
	Q_OBJECT

	QCPGraph* m_Graph;
	QCPItemText* m_Label;

	const H2A::Dataset* m_Dataset;

public:

	TimeSeries(QCustomPlot* plot, const H2A::Dataset* dataset);
	~TimeSeries();

	void setColor(QColor color);

	const std::vector<const H2A::Dataset*> datasets() override;

private slots:
	void updateLabelPosition();

};

