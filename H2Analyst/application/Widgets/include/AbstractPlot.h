#pragma once

#include "Namespace.h"
#include "DataStructures.h"
#include "TimeCursor.h"
#include "AbstractGraph.h"

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

	// Time cursor
	void setTimeCursor(double time);
	virtual void enableTimeCursor(bool enable) {};

protected:

	// Mouse events
	void mouseMoveEvent(QMouseEvent* event) override;
	void leaveEvent(QEvent*) override;
	void wheelEvent(QWheelEvent* event) override;

	// Drag/drop functionality
	virtual void dragEnterEvent(QDragEnterEvent* event) override;
	virtual void dropEvent(QDropEvent* event) override;

	// Context menu
	void createAbstractContextMenu(QMenu& menu);

private slots:
	void enforceViewLimits();
	void contextMenu(const QPoint& pos);

public slots:
	virtual void resetView() {};
	void clear();

signals:

	void mouseMoved(QMouseEvent*);
	void mouseLeft();
	void timeAxisChanged(AbstractPlot*);
	void plotSelected(AbstractPlot* source, H2A::PlotType type);
	void deleteMe(AbstractPlot* source);

};