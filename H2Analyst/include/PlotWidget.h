#pragma once

#include <algorithm>
#include <iomanip> // Used to format double in string with set precision (crosshair labels)


#include <QWidget>
#include <QVector>
#include <QColor>

#include "qcustomplot.h"

#include "DataStructures.h"
#include "DataPanel.h"
#include "PlotCrosshairs.h"
#include "PlotLine.h"

//class DataPanel;

class PlotWidget :
    public QCustomPlot
{

    // Padding around min and max data on the ranges (fractional)
    const float RANGE_PADDING_X = 0.05;
    const float RANGE_PADDING_Y = 0.05;

    Q_OBJECT

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
    std::vector<PlotLine*> m_PlotLines;
    Crosshairs* m_Crosshairs;
    bool m_LegendEnabled;

    QCPRange m_TimeRange;
    QCPRange m_DataRange;
    double m_MaxTimePadding;
    double m_MaxDataPadding;
    

    void plot();

    void clearPlottables();

    void setDataset(const H2A::Dataset* dataset, bool replot = true);
    void setDatasets(std::vector<const H2A::Dataset*> datasets, bool replot = true);
    void addDataset(const H2A::Dataset* dataset, bool replot = true);
    void addDatasets(std::vector<const H2A::Dataset*> datasets, bool replot = true);
    void addPlotLine(const H2A::Dataset* dataset);

    void setAxisLabels();

public:
    PlotWidget(QWidget* parent = nullptr);

    void setDataPanel(const DataPanel* datapanel);
    bool isEmpty();
    void resetView();

protected:
    void leaveEvent(QEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

signals:
    void mouseMoved(QMouseEvent* event);
    void rangeChanged(PlotWidget* source);
    void resetViewsRequested();

private slots:
    void showContextMenu(const QPoint& pos);
    void plotSelected();
    void copyToClipboard();
    void emitRangeChanged() { emit rangeChanged(this); };
    void enforceAxisLimits();

};

