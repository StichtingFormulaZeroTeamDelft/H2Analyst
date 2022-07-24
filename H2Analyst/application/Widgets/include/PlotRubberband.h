#pragma once

#include "AbstractPlot.h"
#include "qcustomplot.h"

#include <QRubberband>

class AbstractPlot;

class Rubberband : public QRubberBand
{

	Q_OBJECT

	const double SNAP_POINT = 20;

	AbstractPlot* m_Plot;
	QPoint m_Origin;
	QPoint m_Mouse;

	bool m_Active;
	bool m_SnappedH;
	bool m_SnappedV;

public:

	Rubberband(AbstractPlot* parent);

	void start(QPoint origin);
	void end();
	void cancel();
	bool isActive() const { return m_Active; };

signals:
	void released(const QCPRange xRange, const QCPRange yRange);

private slots:
	void mouseMoveEvent(QMouseEvent* event);

};
