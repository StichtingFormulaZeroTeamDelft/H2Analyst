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
	~PlotLine();
	
	void setColor(QColor color);

	const H2A::Dataset* dataset() const { return m_Dataset; };

	double minX() const { return rangeX.lower; };
	double maxX() const { return rangeX.upper; };
	double minY() const { return rangeY.lower; };
	double maxY() const { return rangeY.upper; };

private slots:
	void updateLabelPosition();

};

