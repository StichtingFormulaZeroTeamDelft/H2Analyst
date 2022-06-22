#pragma once

#include <QRubberband>

#include "PlotWidget.h"

class Rubberband : public QRubberBand
{

	Q_OBJECT

	const double SNAP_POINT = 20;

	PlotWidget* m_Plot;
	QPoint m_Origin;
	QPoint m_Mouse;

	bool m_Enabled;
	bool m_SnappedH;
	bool m_SnappedV;

public:

	Rubberband(PlotWidget* parent);

	void start(QPoint origin);
	void end(QCPRange& xRange, QCPRange& yRange);
	void cancel();
	const bool isEnabled() const { return m_Enabled; };

signals:
	void released(const QCPRange xRange, const QCPRange yRange);

private slots:
	void mouseMoveEvent(QMouseEvent* event);

};
