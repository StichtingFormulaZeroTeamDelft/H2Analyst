#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QSplitter>
#include <QSizePolicy>
#include <QFileDialog>

#include "ControlPanel.h"
#include "DataStore.h"
#include "DataPanel.h"
#include "Dialogs.h"
#include "PlotManager.h"
#include "SettingsManager.h"
#include "Parsers.h"
#include "PanelToggleButton.h"
#include "Exporters.h"

class PanelToggleButton;

class H2Analyst : public QMainWindow
{
    Q_OBJECT

private:

    const uint16_t SIDEPANEL_WIDTH = 350;

    DataStore* m_DataStore;

    QHBoxLayout* m_HLayout;
    QVBoxLayout* m_VLayout;

    QWidget* m_SidePanel;
    ControlPanel* m_ControlPanel;
    DataPanel* m_DataPanel;
    PlotManager* m_PlotManager;
    SettingsManager* m_SettingsManager;

    PanelToggleButton* m_PbHidePanel;

    QPixmap m_Logo;

public:
    H2Analyst(QWidget* parent = Q_NULLPTR);


public slots:
    void hideSidePanel();
    void openFiles();
    void exportDatasets();

};
