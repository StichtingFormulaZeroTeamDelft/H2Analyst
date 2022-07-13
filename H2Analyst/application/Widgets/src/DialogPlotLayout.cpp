#include "DialogPlotLayout.h"

/**
* Dialog that is used to select the plot layout.
*
* @param parent Parent of this dialog.
**/
DialogPlotLayout::DialogPlotLayout(QWidget* parent) : QDialog(parent, Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint)
{
	this->setModal(true);
	m_SelectedLayout = { 1, 1 };
	this->setWindowTitle("Set layout");

	m_Layout = new QGridLayout(this);
	m_Layout->setSpacing(1);

	for (uint8_t r = 0; r < N_ROWS; ++r) {
		for (uint8_t c = 0; c < N_COLS; ++c) {
			DPLButton* button = new DPLButton(r, c, this);
			button->setFixedSize(30, 30);
			connect(button, SIGNAL(btClicked(DPLButton*)), this, SLOT(btClicked(DPLButton*)));
			connect(button, SIGNAL(btEntered(DPLButton*)), this, SLOT(btEntered(DPLButton*)));
			connect(button, SIGNAL(btLeft(DPLButton*)), this, SLOT(btLeft(DPLButton*)));
			m_Layout->addWidget(button, r, c);
			m_Buttons.push_back(button);
		}
	}
	this->setLayout(m_Layout);
}

/**
* Slot that is called when a button is clicked.
*
* @param source Button that triggered the call.
**/
void DialogPlotLayout::btClicked(DPLButton* source) {
	m_SelectedLayout = { source->row, source->col };
	this->accept();
}

/**
* Slot that is called when the mouse enters a button.
*
* @param source Button that triggered the call.
**/
void DialogPlotLayout::btEntered(DPLButton* source) {
	for (const auto& bt : m_Buttons) {
		if (bt->col <= source->col && bt->row <= source->row) bt->highlight = true;
		else bt->highlight= false;
		bt->repaint();
	}
}
/**
* Slot that is called when the mouse leaves a button.
* 
* @param source Button that triggered the call.
**/
void DialogPlotLayout::btLeft(DPLButton* source) {
	for (const auto& bt : m_Buttons) {
		bt->highlight = false;
		bt->repaint();
	}
}

/**
* Custom button with additional functionality for DialogPlotLayout.
* 
* @param r Row that this button belongs to.
* @param c Column that this button belongs to.
* @param parent Parent widget of this button.
**/
DPLButton::DPLButton(uint8_t& r, uint8_t& c, DialogPlotLayout* parent) : QPushButton(parent),
row(r+1),
col(c+1),
highlight(false)
{
	connect(this, &DPLButton::clicked, [this]() {emit btClicked(this); });
}

/**
* Overrides paint function to control highlighting by member variable.
**/
void DPLButton::paintEvent(QPaintEvent*) {
	QStylePainter p(this);
	QStyleOptionButton option;
	initStyleOption(&option);
	if (highlight) option.state |= QStyle::State_MouseOver;
	p.drawControl(QStyle::CE_PushButton, option);
}
