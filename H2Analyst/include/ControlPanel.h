#pragma once

#include <iostream>

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QCheckBox>

class H2Analyst;

class ControlPanel :
    public QWidget
{

    Q_OBJECT

public:
    ControlPanel(QWidget* parent = nullptr);

private:
    QPushButton* m_BtLoad;
    QPushButton* m_BtPlot;
    QPushButton* m_BtPlotLayout;
    QCheckBox* m_Checkbox;
    
    QGridLayout* m_Layout = new QGridLayout;


signals:
    void pbLoad();
    void pbPlotLayout();
    void alignTimeAxis(bool align);
};

