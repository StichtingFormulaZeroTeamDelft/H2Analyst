#include "DialogPlotLayout.h"


DialogPlotLayout::DialogPlotLayout(QWidget* parent) : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
	this->setModal(true);
	m_SelectedLayout = { 1, 1 };

	m_Layout = new QGridLayout(this);
	m_Layout->setSpacing(1);

	for (uint8_t r = 0; r < N_ROWS; ++r)
	{
		for (uint8_t c = 0; c < N_COLS; ++c)
		{
			DialogPlotLayoutButton* button = new DialogPlotLayoutButton(r, c, this);
			button->setFixedSize(30, 30);
			connect(button, SIGNAL(btClicked(DialogPlotLayoutButton*)), this, SLOT(buttonClicked(DialogPlotLayoutButton*)));
			m_Layout->addWidget(button, r, c);
		}
	}

	this->setLayout(m_Layout);

}


void DialogPlotLayout::buttonClicked(DialogPlotLayoutButton* sender)
{
	m_SelectedLayout = { sender->m_Row, sender->m_Col };
	this->accept();
}


DialogPlotLayoutButton::DialogPlotLayoutButton(uint8_t& row, uint8_t& col, QWidget* parent) : QPushButton(parent)
{
	m_Row = row + 1;
	m_Col = col + 1;
	connect(this, &DialogPlotLayoutButton::clicked, [this]() {emit btClicked(this); });

}
