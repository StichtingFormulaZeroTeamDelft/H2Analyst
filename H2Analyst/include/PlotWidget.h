#pragma once

#include <algorithm>
#include <iomanip> // Used to format double in string with set precision (crosshair labels)


#include <QWidget>
#include <QVector>
#include <QColor>

#include "qcustomplot.h"

#include "DataStructures.h"
#include "DataPanel.h"
#include "TimeSeries.h"
#include "XYSeries.h"
#include "PlotCrosshairs.h"
#include "PlotManager.h"
#include "TimeCursor.h"

class PlotManager;
class Crosshairs;
class TimeCursor;

class PlotWidget :
    public QCustomPlot
{
    Q_OBJECT

public:
    
    enum class PlotType {
        Time, XY
    };

private:

    PlotManager* m_PlotManager;
    PlotType m_Type;

    // Padding around min and max data on the ranges (fractional)
    const float RANGE_PADDING_X = 0.05f;
    const float RANGE_PADDING_Y = 0.05f;

    const std::vector<QColor> k_PlotColors = {
    QColor(0, 114, 189),
    QColor(217, 83, 25),
    QColor(237, 177, 32),
    QColor(126, 47, 142),
    QColor(119, 172, 48),
    QColor(77, 190, 238),
    QColor(162, 20, 47),
    };

    const DataPanel* m_DataPanel;
    std::vector<Plottable*> m_Plottables;
    Crosshairs* m_Crosshairs;
    TimeCursor* m_TimeCursor;
    bool m_LegendEnabled;

    QCPRange m_RangeLimitX;
    QCPRange m_RangeLimitY;

    void setPlots(std::vector<const H2A::Dataset*> datasets, PlotType type = PlotType::Time);
    void addPlots(std::vector<const H2A::Dataset*> datasets, PlotType type = PlotType::Time);

    void setAxisLabels();
    void restrictView(const QCPRange& oldRange, const QCPRange& newRange, QCPAxis* axis);
    void updateRangeAndPadding();
    void dataRangeSpan(QCPRange& xRange, QCPRange& yRange);
    QColor nextColor() { return k_PlotColors[m_Plottables.size() % k_PlotColors.size()]; };


public:
    PlotWidget(PlotManager* plotManager);

    void setDataPanel(const DataPanel* datapanel);
    bool isEmpty();
    void zoomYToData();
    void resetView(bool x = true, bool y = true);
    bool timeAxisAlignable() { return m_Type == PlotType::Time; };

    const QCPRange currentRangeX() const { return this->xAxis->range(); };
    const QCPRange currentRangeY() const { return this->yAxis->range(); };
    const PlotWidget::PlotType type() const { return m_Type; };
    const std::vector<Plottable*> plottables() const { return this->m_Plottables; };
    TimeCursor* timeCursor() { return m_TimeCursor; };

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void leaveEvent(QEvent* event);

signals:
    void timeAxisChanged(PlotWidget* source);
    void plottablesChanged(PlotWidget* source);
    void mouseMoved(QMouseEvent* event);
    void wheelMoved(QWheelEvent* event);
    void mouseLeft();

private slots:
    void clear();
    void showContextMenu(const QPoint& pos);
    void plotSelected(PlotType type);
    void copyToClipboard();

    // Slots to deal with axis value changing
    void emitTimeAxisChanged() { emit this->timeAxisChanged(this); }; // This function is needed because a lambda function can not be connected to an overloaded signal. It's a flaw of Qt.
    void restrictViewX(const QCPRange& newRange, const QCPRange& oldRange) { this->restrictView(newRange, oldRange, this->xAxis); };
    void restrictViewY(const QCPRange& newRange, const QCPRange& oldRange) { this->restrictView(newRange, oldRange, this->yAxis); };

};

