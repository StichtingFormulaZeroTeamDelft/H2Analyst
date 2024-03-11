#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QStylePainter>
#include <QStyleOptionButton>


#include <iostream>
#include <vector>

class DPLButton;

class DialogPlotLayout :
	public QDialog
{

	Q_OBJECT

	const uint8_t N_COLS = 4;
	const uint8_t N_ROWS = 4;

	QGridLayout* m_Layout;
	std::vector<DPLButton*> m_Buttons;

public:
		
	std::vector<uint8_t> m_SelectedLayout;
	DialogPlotLayout(QWidget* parent = nullptr);

private slots:
	void btClicked(DPLButton* source);
	void btEntered(DPLButton* source);
	void btLeft(DPLButton* source);

};


class DPLButton : public QPushButton
{

	Q_OBJECT

public:
	uint8_t row;
	uint8_t col;
	bool highlight;
	DPLButton(uint8_t& r, uint8_t& c, DialogPlotLayout* parent = nullptr);

protected:
	void paintEvent(QPaintEvent* event) override;
	void enterEvent(QEnterEvent* event) override { emit this->btEntered(this); };
	void leaveEvent(QEvent* event) override { emit this->btLeft(this); };

signals:
	void btClicked(DPLButton* source);
	void btEntered(DPLButton* source);
	void btLeft(DPLButton* source);

};

