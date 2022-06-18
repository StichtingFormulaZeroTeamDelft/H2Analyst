#pragma once

#include <QObject>

#include "qcustomplot.h"

#include "PlotWidget.h"

class PlotWidget;

class TimeCursor : public QObject
{
	Q_OBJECT

	PlotWidget* m_Plot;
	QCPLayer* m_Layer;
	double m_Time;
	bool m_Enabled;
	QCPItemLine* m_LineV;
	std::vector<QCPItemText*> m_Labels;

	void draw();

public:

	TimeCursor(PlotWidget* plot);

	bool enabled() { return m_Enabled; };

public slots:
	void setEnabled(bool enabled);
	void enable();
	void disable();
	void setTime(double time);

};
