#pragma once

#include "Plottable.h"
#include "DataStructures.h"
#include "DataOperations.h"

class TimeSeries : public Plottable
{
	Q_OBJECT

	QCPGraph* m_Graph;
	QCPItemText* m_Label;
	const H2A::Dataset* m_Dataset;

public:

	TimeSeries(QCustomPlot* plot, const H2A::Dataset* dataset);
	~TimeSeries();

	void setColor(QColor color) override;

	const QPointF dataAt(double time) const override;
	const bool boundedRangeY(const QCPRange bounds, QCPRange& range) const override;

private slots:
	void updateLabelPosition();

};

