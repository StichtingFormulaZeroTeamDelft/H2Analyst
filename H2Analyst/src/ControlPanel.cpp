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
	connect(m_LeTimeCursor, &QLineEdit::returnPressed, [=]() {
		emit this->setTimeCursorTime(std::stod(m_LeTimeCursor->text().toStdString()));
		m_CbTimeCursor->setChecked(true);
		});

	m_Layout->addWidget(m_BtLoad);
	m_Layout->addWidget(m_BtPlotLayout);
	//m_Layout->addWidget(m_BtExport);
	m_Layout->addWidget(m_TbTimeAlign);
	m_Layout->addWidget(m_CbTimeCursor);
	m_Layout->addWidget(m_LeTimeCursor);

	this->setLayout(m_Layout);

}


void ControlPanel::setTimeCursorTimeInputbox(double time) {
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << time;
	m_LeTimeCursor->setText(QString(ss.str().c_str()));
	m_CbTimeCursor->setChecked(true);
}
