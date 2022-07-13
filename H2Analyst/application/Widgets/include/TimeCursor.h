#pragma once

#include "AbstractPlot.h"

#include "qcustomplot.h"

#include <QObject>

#include <sstream>
#include <iomanip> // Used to set presision of stringstream

// Forward declarations
class AbstractPlot;

class TimeCursor : public QObject
{
	Q_OBJECT

	AbstractPlot* m_Plot;
	QCPLayer* m_Layer;
	double m_Time;
	bool m_Enabled;
	QCPItemLine* m_LineV;
	std::vector<QCPItemText*> m_Labels;


public:

	TimeCursor(AbstractPlot* plot);

	bool enabled() { return m_Enabled; };

public slots:
	void draw();
	void setEnabled(bool enabled);
	void enable() { this->setEnabled(true); };
	void disable() { this->setEnabled(false); };
	void setTime(double time);

};
