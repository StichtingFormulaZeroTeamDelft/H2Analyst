#pragma once

#include "PlotWidget.h"
#include "DataPanel.h"
#include "DialogPlotLayout.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <vector>

class PlotWidget;

class PlotManager :
    public QWidget
{

    Q_OBJECT

private:        

    QVBoxLayout* m_VLayout;
    std::vector<QHBoxLayout*> m_HLayouts;

    const DataPanel* m_DataPanel;

    bool m_Empty;
    bool m_AlignTimeAxis;
    bool m_BusyAligning;
    QCPRange m_TimeRange;

    std::vector<PlotWidget*> m_Plots;

    PlotWidget* createPlot();
    void clearLayout(QLayout* layout);
    void setPlotLayoutRC(uint8_t rows, uint8_t cols);
    void resetViews();


public:

    PlotManager(QWidget *parent = nullptr);
    void setDataPanel(const DataPanel* datapanel);
    bool aligningTimeAxis() { return m_AlignTimeAxis; };
    bool empty() { return m_Empty; };


public slots:
    void setPlotLayoutDialog();
    void plotTimeAxisChanged(PlotWidget* source);
    void alignTimeAxis(bool align);

private slots:
    void plotDataAdded(PlotWidget* source);
    void plotDataRemoved(PlotWidget* source);

};
