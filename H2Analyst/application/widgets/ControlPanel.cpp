#include "ControlPanel.h"

ControlPanel::ControlPanel(QWidget* parent) : QWidget(parent)
{
	m_BtLoad = new QPushButton(tr("Load"));
	connect(m_BtLoad, SIGNAL(clicked()), this, SIGNAL(pbLoad()));

	m_BtPlotLayout = new QPushButton(tr("Set layout"));
	connect(m_BtPlotLayout, SIGNAL(clicked()), this, SIGNAL(pbPlotLayout()));

	m_BtExport = new QPushButton(tr("Export"));
	connect(m_BtExport, SIGNAL(clicked()), this, SIGNAL(pbExport()));

	m_TbTimeAlign = new QToolButton(this);
	m_TbTimeAlign->setCheckable(true);
	m_TbTimeAlign->setAutoRaise(true);
	m_TbTimeAlign->setChecked(true);
	m_TbTimeAlign->setIcon(QIcon(QPixmap(":/icons/tb-time-align")));
	m_TbTimeAlign->setToolTip("Align time axis of all plots");
	connect(m_TbTimeAlign, &QToolButton::clicked, [=]() { emit setTimeAlignEnable(m_TbTimeAlign->isChecked()); });

	m_CbTimeCursor = new QCheckBox("Enable time cursor", this);
	m_CbTimeCursor->setCheckState(Qt::Unchecked);
	connect(m_CbTimeCursor, &QCheckBox::stateChanged, [=]() { emit setTimeCursorEnable(m_CbTimeCursor->checkState() == Qt::Checked); });

	m_LeTimeCursor = new QLineEdit(this);
	m_LeTimeCursor->setClearButtonEnabled(true);
	connect(m_LeTimeCursor, SIGNAL(returnPressed()), this, SLOT(timeCursorTimeEntered()));

	m_RbForze8 = new QRadioButton("Forze 8", this);
	connect(m_RbForze8, &QRadioButton::toggled, [=](bool checked) { if (checked) emit selectedCarChanged(H2A::Car::Forze8); });

	m_RbForze9 = new QRadioButton("Forze 9", this);
	m_RbForze9->setChecked(true);
	connect(m_RbForze9, &QRadioButton::toggled, [=](bool checked) { if (checked) emit selectedCarChanged(H2A::Car::Forze9); });

	m_Layout->addWidget(m_BtLoad, 0, 0, 1, 2);
	m_Layout->addWidget(m_BtPlotLayout, 1, 0, 1, 2);
	//m_Layout->addWidget(m_BtExport, 2, 0, 1, 2);
	m_Layout->addWidget(m_TbTimeAlign, 3, 0, 1, 2);
	m_Layout->addWidget(m_CbTimeCursor, 4, 0, 1, 2);
	m_Layout->addWidget(m_LeTimeCursor, 5, 0, 1, 2);
	m_Layout->addWidget(m_RbForze8, 6, 0, 1, 1);
	m_Layout->addWidget(m_RbForze9, 6, 1, 1, 1);

	this->setLayout(m_Layout);
}


void ControlPanel::setTimeCursorTimeInputbox(double time) {
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << time;
	m_LeTimeCursor->setText(QString(ss.str().c_str()));
	m_CbTimeCursor->setChecked(true);
}


/**
* Used to check if input time is valid and if so, set cursor to this time.
**/
void ControlPanel::timeCursorTimeEntered() {

	double time = 0.0;

	// Check if input is a valid double
	char* end;
	time = std::strtod(m_LeTimeCursor->text().toStdString().c_str(), &end);

	// Check box and set time
	m_CbTimeCursor->setChecked(true);
	emit this->setTimeCursorTime(time);
}
