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
    QCheckBox* m_CbTimeAlign;
    QCheckBox* m_CbTimeCursor;
    QLineEdit* m_LeTimeCursor;
    
    QGridLayout* m_Layout = new QGridLayout;

signals:
    void pbLoad();
    void pbPlotLayout();
    void pbExport();
    void setTimeAlignEnable(bool align);
    void setTimeCursorEnable(bool align);
    void setTimeCursorTime(double time);

public slots:
    void setTimeCursorTimeInputbox(double time);
};

