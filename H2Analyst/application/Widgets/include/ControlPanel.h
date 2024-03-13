#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QToolButton>
#include <QRadioButton>

#include "Namespace.h"

class H2Analyst;

class ControlPanel :
    public QWidget
{

    Q_OBJECT

public:
    ControlPanel(QWidget* parent = nullptr);

private:
    QPushButton* m_BtLoad;
    QPushButton* m_BtPlotLayout;
    QPushButton* m_BtExport;
    QToolButton* m_TbTimeAlign;
    QCheckBox* m_CbTimeCursor;
    QLineEdit* m_LeTimeCursor;
    QRadioButton* m_RbForze8;
    QRadioButton* m_RbForze9;
    
    QGridLayout* m_Layout = new QGridLayout;

signals:
    void pbLoad();
    void pbPlotLayout();
    void pbExport();
    void setTimeAlignEnable(bool align);
    void setTimeCursorEnable(bool align);
    void setTimeCursorTime(double time);
    void selectedCarChanged(H2A::Car car);

private slots:
    void timeCursorTimeEntered();

public slots:
    void setTimeCursorTimeInputbox(double time);
};

