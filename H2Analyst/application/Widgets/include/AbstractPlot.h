#pragma once

#include "Namespace.h"
#include "TimeCursor.h"
#include "PlotManager.h"

#include "qcustomplot.h"

// Forward declarations
class PlotManager;
class TimeCursor;
class Crosshairs;
class Rubberband;

/**
* Base class for any plot widget.
**/
class AbstractPlot : public QCustomPlot
{

	Q_OBJECT

	PlotManager* m_PlotManager;
	H2A::PlotType m_Type;

	TimeCursor* m_TimeCursor;

public:

	AbstractPlot(PlotManager* manager);

	// Properties
	virtual H2A::PlotType type() const { return m_Type; };
	virtual bool isEmpty() const { return true; }
	
	// Actions
	virtual void resetView() {};

	// Time cursor
	void setTimeCursor(double time);
	virtual void enableTimeCursor(bool enable) {};

protected:
	void mouseMoveEvent(QMouseEvent* event) override;
	void leaveEvent(QEvent*) override;

signals:

	void timeAxisChanged(AbstractPlot*);
	void mouseMoved(QMouseEvent*);
	void mouseLeft();

};