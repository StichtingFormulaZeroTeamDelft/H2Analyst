#pragma once

#include "AbstractPlot.h"
#include "DataPanel.h"
#include "DialogPlotLayout.h"
#include "Dialogs.h"
#include "Namespace.h"

#include "qcustomplot.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QSplitter>

#include <vector>

class AbstractPlot;

class PlotManager :
    public QWidget
{

    Q_OBJECT

private:        

    QVBoxLayout* m_Layout;
    QSplitter* m_VSplitter;
    std::vector<QSplitter*> m_HSplitters;

    const DataPanel* m_DataPanel;
    std::vector<AbstractPlot*> m_Plots;

    bool m_TimeAlignEnabled;
    bool m_BusyAligning;
    
    double m_TimeCursorTime;
    bool m_TimeCursorEnabled;

    AbstractPlot* createPlot();
    void clearLayout(QLayout* layout);
    void setPlotLayoutRC(uint8_t rows, uint8_t cols);
    void alignTimeAxis(AbstractPlot* ref = nullptr);

public:

    PlotManager(QWidget *parent = nullptr);
    void setDataPanel(const DataPanel* datapanel) { m_DataPanel = datapanel; };
    bool aligningTime() { return m_TimeAlignEnabled; };
    bool allPlotsEmpty();
    bool getOtherTimeRange(AbstractPlot* source, QCPRange& range) const;
    const bool timeCursorEnabled() const { return m_TimeCursorEnabled; };
    const double timeCursorTime() const { return m_TimeCursorTime; };
    size_t numberOfPlots() { return m_Plots.size(); };

public slots:
    void setPlotLayoutDialog();
    void setTimeAlignEnabled(bool align);
    void setTimeCursorEnabled(bool enabled);
    void setTimeCursorTime(double time);
    void resetAllViews();
    void timeAxisChanged(AbstractPlot* source);
    void deletePlot(AbstractPlot* source);
    void insertPlot(AbstractPlot* source, H2A::Direction dir);
    
signals:
    void timeCursorMoved(double time);

};