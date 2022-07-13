#pragma once

#include "AbstractPlot.h"

#include <QRubberband>


class Rubberband : public QRubberBand
{

	Q_OBJECT

	const double SNAP_POINT = 20;

	AbstractPlot* m_Plot;
	QPoint m_Origin;
	QPoint m_Mouse;

	bool m_Enabled;
	bool m_SnappedH;
	bool m_SnappedV;

public:

	Rubberband(AbstractPlot* parent);

	void start(QPoint origin);
	void end(QCPRange& xRange, QCPRange& yRange);
	void cancel();
	const bool isEnabled() const { return m_Enabled; };

signals:
	void released(const QCPRange xRange, const QCPRange yRange);

private slots:
	void mouseMoveEvent(QMouseEvent* event);

};
