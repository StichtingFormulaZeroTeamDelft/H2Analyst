#pragma once

#include <algorithm>

#include "DataStructures.h"
#include "qcustomplot.h"

class PlotLine : public QObject
{
	Q_OBJECT

	QCustomPlot* m_Parent;
	const H2A::Dataset* m_Dataset;
	QCPGraph* m_Graph;
	QCPItemText* m_Label;
	QColor m_Color;

	QCPRange rangeX;
	QCPRange rangeY;

public:

	PlotLine(QCustomPlot* parentPlot, const H2A::Dataset* dataset);
	
	void setColor(QColor color);

	double minX() { return rangeX.lower; };
	double maxX() { return rangeX.upper; };
	double minY() { return rangeY.lower; };
	double maxY() { return rangeY.upper; };

private slots:
	void updateLabelPosition();

};

