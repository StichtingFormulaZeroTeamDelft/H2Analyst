#pragma once

#include "Namespace.h"
#include "DataStructures.h"
#include "TimeCursor.h"
#include "PlotRubberband.h"
#include "AbstractGraph.h"
#include <QtGlobal>

#include "qcustomplot.h"

// Forward declarations
class TimeCursor;
class Crosshairs;
class Rubberband;

/**
* Base class for any plot widget.
**/
class AbstractPlot : public QCustomPlot
{

	Q_OBJECT
	
protected:
	H2A::PlotType m_Type;
	std::vector<AbstractGraph*> m_Graphs;
	TimeCursor* m_TimeCursor;
	Rubberband* m_Rubberband;


	QCPRange m_LimHardX, m_LimHardY;
	bool m_BusyEnforcingViewLimits;

	virtual void setAxisLabels() {};

public:

	AbstractPlot(QWidget* parent);

	// Getters/Setters
	H2A::PlotType type() const { return m_Type; };
	std::vector<const H2A::Dataset*> datasets() const;
	std::vector<AbstractGraph*> graphs() const { return m_Graphs; };

	// Properties
	virtual bool isEmpty() const { return m_Graphs.size() == 0; }
	virtual QCPRange dataRangeX() const;
	virtual QCPRange dataRangeY() const;
	
	// Actions
	virtual void plot(std::vector<const H2A::Dataset*> datasets, bool clearFirst = false);
	virtual void replot();


protected:

	// Mouse events
	void mouseMoveEvent(QMouseEvent* event) override;
	void leaveEvent(QEvent*) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

	// Drag/drop functionality
	virtual void dragEnterEvent(QDragEnterEvent* event) override;
	virtual void dropEvent(QDropEvent* event) override;

private slots:
	void enforceViewLimits();
	void emitTimeAxisChanged();

public slots:
	virtual void resetView() {};
	void clear();
	void clip();

	// Time cursor
	virtual void setTimeCursorEnabled(bool enable);
	virtual void setTimeCursorTime(double time);

signals:
	void contextMenuRequested(AbstractPlot* source, const QPoint& pos);
	void mouseMoved(QMouseEvent*);
	void mouseLeft();
	void timeAxisChanged(AbstractPlot*);
	void plotSelected(AbstractPlot* source, H2A::PlotType type, bool clearFirst = true);
	void timeCursorPlaced(double time);
};