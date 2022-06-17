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
    std::vector<PlotWidget*> m_Plots;

    bool m_AlignTimeAxis;
    bool m_BusyAligning;
    
    double m_TimeCursorTime;
    bool m_TimeCursorEnabled;

    PlotWidget* createPlot();
    void clearLayout(QLayout* layout);
    void setPlotLayoutRC(uint8_t rows, uint8_t cols);
    void resetViews();

public:

    PlotManager(QWidget *parent = nullptr);
    void setDataPanel(const DataPanel* datapanel);
    bool aligningTimeAxis() { return m_AlignTimeAxis; };
    bool isEmpty();


public slots:
    void setPlotLayoutDialog();
    void alignTimeAxis(PlotWidget* source);
    void setAlignTimeAxis(bool align);
    void setTimeCursorEnabled(bool enabled);
    void setTimeCursorTime(double time);


};
