#include "H2Analyst.h"

H2Analyst::H2Analyst(QWidget* parent)
    : QMainWindow(parent),
    m_SidePanel(new QWidget),
    m_DataStore(new DataStore),
    m_ControlPanel(new ControlPanel),
    m_DataPanel(new DataPanel),
    m_PlotManager(new PlotManager),
    m_VLayout(new QVBoxLayout),
    m_HLayout(new QHBoxLayout),
    m_PbHidePanel(new PanelToggleButton(24)),
    m_Logo(":/H2Analyst/forze_logo")
{
    
    // Build left-side panel
    QLabel* logoLbl = new QLabel;
    logoLbl->setPixmap(m_Logo.scaledToWidth((int)SIDEPANEL_WIDTH/1.5, Qt::SmoothTransformation));
    logoLbl->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    m_VLayout->addWidget(logoLbl);
    m_VLayout->addWidget(m_ControlPanel);
    m_VLayout->addWidget(m_DataPanel);
    m_SidePanel->setMaximumWidth(SIDEPANEL_WIDTH);
    m_SidePanel->setLayout(m_VLayout);

    // Panel toggle button
    m_PbHidePanel->setOpenIcon(QPixmap(":/icons/right-arrow"));
    m_PbHidePanel->setCloseIcon(QPixmap(":/icons/left-arrow"));

    // Assemble component and put in central widget
    m_HLayout->addWidget(m_SidePanel);
    m_HLayout->addWidget(m_PbHidePanel);
    m_PbHidePanel->updateIcon();
    m_HLayout->addWidget(m_PlotManager);
    m_HLayout->setSpacing(0);
    m_HLayout->setContentsMargins(0, 0, 0, 0);
    QWidget *mainWidget = new QWidget;
    mainWidget->setLayout(m_HLayout);
    this->setCentralWidget(mainWidget);


    // -------- All objects constructed --------- //

    // Create cross-references
    m_DataPanel->setDataStore(m_DataStore);
    m_PlotManager->setDataPanel(m_DataPanel);

    // Connect signals to slots
    connect(m_PbHidePanel, SIGNAL(clicked()), this, SLOT(hideSidePanel()));
    connect(m_DataStore, SIGNAL(fileLoaded()), m_DataPanel, SLOT(updateData()));
    
    connect(m_ControlPanel, SIGNAL(pbLoad()), this, SLOT(openFiles()));
    connect(m_ControlPanel, SIGNAL(pbPlotLayout()), m_PlotManager, SLOT(setPlotLayoutDialog()));
    connect(m_ControlPanel, SIGNAL(pbExport()), this, SLOT(exportDatasets()));
    connect(m_ControlPanel, SIGNAL(setTimeAlignEnable(bool)), m_PlotManager, SLOT(setAlignTimeAxisEnabled(bool)));
    connect(m_ControlPanel, SIGNAL(setTimeCursorEnable(bool)), m_PlotManager, SLOT(setTimeCursorEnabled(bool)));
    connect(m_ControlPanel, SIGNAL(setTimeCursorTime(double)), m_PlotManager, SLOT(setTimeCursorTime(double)));
    connect(m_PlotManager, SIGNAL(timeCursurTimeChanged(double)), m_ControlPanel, SLOT(setTimeCursorTimeInputbox(double)));


    std::cout << "H2Analyst has been started" << std::endl;
}


void H2Analyst::hideSidePanel()
{
    if (m_PbHidePanel->getState() == PanelToggleButton::Open)
    {
        // Hide
        QPropertyAnimation* animation = new QPropertyAnimation(m_SidePanel, "maximumWidth");
        animation->setDuration(200);
        animation->setStartValue(SIDEPANEL_WIDTH);
        animation->setEndValue(0);

        animation->start();
        m_PbHidePanel->toggle();
    }
    else
    {
        // Show
        QPropertyAnimation* animation = new QPropertyAnimation(m_SidePanel, "maximumWidth");
        animation->setDuration(200);
        animation->setStartValue(0);
        animation->setEndValue(SIDEPANEL_WIDTH);

        animation->start();
        m_PbHidePanel->toggle();
    }
}


void H2Analyst::exportDatasets()
{
    const std::vector<const H2A::Dataset*> datasets = m_DataPanel->getSelectedDatasets();
    for (const auto& dataset : datasets) {
        if (!dataset->populated) m_DataPanel->requestDatasetPopulation(dataset, true);
    }
    H2A::Export::CSV(datasets, "test.csv");
}

void H2Analyst::openFiles()
{
    QFileDialog dialog(this);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter("Car data (*.mat)");

    QStringList filenames;
    if (dialog.exec())
    {
        filenames = dialog.selectedFiles();
    }

    m_DataStore->loadFiles(filenames);
}
