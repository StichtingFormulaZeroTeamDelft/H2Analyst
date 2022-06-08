#pragma once


#include "qcustomplot.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip> // Used to format double in string with set precision (crosshair labels)


class Crosshairs : public QObject
{
	Q_OBJECT

	QCustomPlot* m_Parent;
	QCPLayer* m_Layer;
	QPointF m_Pos;
	bool m_Enabled;

	QCPItemLine *m_LineH;
	QCPItemLine *m_LineV;
	QCPItemText* m_LabelX;
	QCPItemText* m_LabelY;

public:

	Crosshairs(QCustomPlot* parentPlot, QString layerName, QCPLayer* layerBelow);

	void enable();
	bool enabled() { return m_Enabled; };
	void disable();
	void update();

public slots:
	void update(QMouseEvent* event);

};