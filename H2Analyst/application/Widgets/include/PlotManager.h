#pragma once

#include "AbstractPlot.h"
#include "TimePlot.h"
#include "EmcyPlot.h"
#include "DataPanel.h"
#include "DialogPlotLayout.h"
#include "Dialogs.h"
#include "Namespace.h"
#include "FlexGridLayout.h"

#include "qcustomplot.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QSplitter>

#include <vector>

class AbstractPlot;
class TimePlot;
class FlexGridLayout;

class PlotManager :
    public QWidget
{

    Q_OBJECT

private:        

    FlexGridLayout* m_Layout;
    //QVBoxLayout* m_Layout;
    //QSplitter* m_VSplitter;
    //std::vector<QSplitter*> m_HSplitters;

    const DataPanel* m_DataPanel;

    bool m_TimeAlignEnabled;
    bool m_BusyAligning;
    
    double m_TimeCursorTime;
    bool m_TimeCursorEnabled;

    H2A::Car m_SelectedCar;

    std::vector<AbstractPlot*> plots();

    void setPlotLayout(uint8_t rows, uint8_t cols);
    AbstractPlot* replacePlot(AbstractPlot* source, H2A::PlotType newType);

public:

    PlotManager(QWidget *parent = nullptr);
    AbstractPlot* createPlot(H2A::PlotType type = H2A::Abstract);
    void setDataPanel(const DataPanel* datapanel) { m_DataPanel = datapanel; };
    bool aligningTime() { return m_TimeAlignEnabled; };
    bool allPlotsEmpty();
    const bool timeCursorEnabled() const { return m_TimeCursorEnabled; };
    const double timeCursorTime() const { return m_TimeCursorTime; };

public slots:
    void contextMenu(AbstractPlot* source, const QPoint& pos);
    void setPlotLayoutDialog();
    void setTimeAlignEnabled(bool align);
    void alignTimeAxis(AbstractPlot* ref = nullptr);
    void setTimeCursorEnabled(bool enabled);
    void setTimeCursorTime(double time);
    void resetAllViews();
    void deletePlot(AbstractPlot* source);
    void insertPlot(AbstractPlot* source, H2A::Direction dir);
    void plotSelected(AbstractPlot* target = nullptr, H2A::PlotType type = H2A::Abstract, bool clearFirst = true);
    void setSelectedCar(H2A::Car car);
    
signals:
    void timeCursorMoved(double time);
    void selectedCarChanged(H2A::Car car);
};