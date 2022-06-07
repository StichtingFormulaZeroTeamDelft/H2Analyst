#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QPushButton>

#include <iostream>
#include <vector>

class DialogPlotLayoutButton;

class DialogPlotLayout :
	public QDialog
{

	Q_OBJECT

	const uint8_t N_COLS = 4;
	const uint8_t N_ROWS = 4;


	QGridLayout* m_Layout;

public:
		
	std::vector<uint8_t> m_SelectedLayout;
	DialogPlotLayout(QWidget* parent = nullptr);

private slots:
	void buttonClicked(DialogPlotLayoutButton* sender);

};


class DialogPlotLayoutButton : public QPushButton
{

	Q_OBJECT

public:
	uint8_t m_Row;
	uint8_t m_Col;
	DialogPlotLayoutButton(uint8_t& row, uint8_t& col, QWidget* parent = nullptr);

signals:
	void btClicked(DialogPlotLayoutButton* sender);

};

