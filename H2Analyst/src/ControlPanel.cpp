#include "ControlPanel.h"

ControlPanel::ControlPanel(QWidget* parent) : QWidget(parent)
{
	m_BtLoad = new QPushButton(tr("Load"));
	connect(m_BtLoad, SIGNAL(clicked()), this, SIGNAL(pbLoad()));

	m_BtPlotLayout = new QPushButton(tr("Set layout"));
	connect(m_BtPlotLayout, SIGNAL(clicked()), this, SIGNAL(pbPlotLayout()));

	m_Checkbox = new QCheckBox("Align time axis", this);
	m_Checkbox->setCheckState(Qt::Checked);
	connect(m_Checkbox, &QCheckBox::stateChanged, [=]() { emit alignTimeAxis(m_Checkbox->checkState() == Qt::Checked); });

	m_Layout->addWidget(m_BtLoad);
	m_Layout->addWidget(m_BtPlotLayout);
	m_Layout->addWidget(m_Checkbox);

	this->setLayout(m_Layout);

}
