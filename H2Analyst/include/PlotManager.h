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

    bool m_AlignTimeAxisEnabled;
    bool m_BusyAligning;
    
    double m_TimeCursorTime;
    bool m_TimeCursorEnabled;

    PlotWidget* createPlot();
    void clearLayout(QLayout* layout);
    void setPlotLayoutRC(uint8_t rows, uint8_t cols);
    void alignTimeAxis(PlotWidget* ref = nullptr);

public:

    PlotManager(QWidget *parent = nullptr);
    void setDataPanel(const DataPanel* datapanel);
    bool aligningTimeAxis() { return m_AlignTimeAxisEnabled; };
    bool isEmpty();
    bool getOtherTimeRange(PlotWidget* source, QCPRange& range) const;

public slots:
    void setPlotLayoutDialog();
    void setAlignTimeAxisEnabled(bool align);
    void setTimeCursorEnabled(bool enabled);
    void setTimeCursorTime(double time);
    void resetAllViews();
    void timeAxisChanged(PlotWidget* source);


};
