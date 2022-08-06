#pragma once

#include "AbstractPlot.h"
#include "PlotManager.h"
#include "Namespace.h"

#include <QGridLayout>
#include <QSplitter>

#include <vector>

class PlotManager;

class FlexGridLayout : public QGridLayout
{

	Q_OBJECT

	PlotManager* m_PlotManager;
	QSplitter* m_VSplitter;
	std::vector<QSplitter*> m_HSplitters;

public:
	FlexGridLayout(PlotManager* parent = nullptr);

	void setLayout(int rows, int cols);
	QWidget* replaceWidget(QWidget* oldWidget, QWidget* newWidget);
	void insertWidget(QWidget* reference, QWidget* newWidget, H2A::Direction dir);
	std::vector<QWidget*> widgets() const;
	std::vector<AbstractPlot*> plots() const;
};

