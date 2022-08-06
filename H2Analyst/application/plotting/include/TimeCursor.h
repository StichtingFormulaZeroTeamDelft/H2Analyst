#pragma once

#include "AbstractPlot.h"

#include "qcustomplot.h"

#include <QObject>

#include <sstream>
#include <iomanip> // Used to set presision of stringstream

// Forward declarations
class AbstractPlot;

class TimeCursor : public QWidget
{
	Q_OBJECT

	const int HITBOX_RANGE = 5;

	AbstractPlot* m_Plot;
	QCPLayer* m_Layer;
	double m_Time;
	bool m_Enabled;
	QCPItemLine* m_LineV;
	std::vector<QCPItemText*> m_Labels;
	bool m_Dragging;


public:

	TimeCursor(AbstractPlot* plot);

	bool enabled() { return m_Enabled; };

protected:
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

public slots:
	void draw();
	void setEnabled(bool enabled);
	void enable() { this->setEnabled(true); };
	void disable() { this->setEnabled(false); };
	void setTime(double time);

};
