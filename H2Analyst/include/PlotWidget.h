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
#include "PlotCrosshairs.h"

//class DataPanel;

class Crosshairs;

class PlotWidget :
    public QCustomPlot
{
    Q_OBJECT

public:
    
    enum class PlotType {
        Time, XY
    };

private:

    PlotType m_Type;

    // Padding around min and max data on the ranges (fractional)
    const float RANGE_PADDING_X = 0.05;
    const float RANGE_PADDING_Y = 0.05;

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
    bool m_LegendEnabled;

    QCPRange m_RangeX;
    QCPRange m_RangeY;
    double m_PaddingX;
    double m_PaddingY;
    

    void plot();

    void setPlots(std::vector<const H2A::Dataset*> datasets, bool replot = true);
    void addPlots(std::vector<const H2A::Dataset*> datasets, bool replot = true);

    void setAxisLabels();

public:
    PlotWidget(QWidget* parent = nullptr);

    void setDataPanel(const DataPanel* datapanel);
    bool isEmpty();
    void resetView();

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void leaveEvent(QEvent* event);

signals:
    void mouseMoved(QMouseEvent* event);
    void wheelMoved(QWheelEvent* event);
    void mouseLeft();
    void timeRangeChanged(PlotWidget* source);
    void resetViewsRequested();

private slots:
    void clear();
    void showContextMenu(const QPoint& pos);
    void plotSelected();
    void copyToClipboard();
    void restrictView(const QCPRange& oldRange, const QCPRange& newRange);
    void emitTimeRangeChanged() { emit this->timeRangeChanged(this); };

};

