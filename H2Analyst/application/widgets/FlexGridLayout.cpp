#include "FlexGridLayout.h"

/**
* FlexGridLayout implements a QGridLayout with flexible widget sizes.
* With splitters, the flexibility is realized while a QGridLayout manages the overall layout.
**/
FlexGridLayout::FlexGridLayout(PlotManager* plotManager) : QGridLayout(plotManager),
m_PlotManager(plotManager),
m_VSplitter(new QSplitter(Qt::Vertical, m_PlotManager)),
m_HSplitters()
{
	this->setLayout(1, 1);
}

/**
* Get a vector with all widgets contained in this layout.
**/
std::vector<QWidget*> FlexGridLayout::widgets() const {
	std::vector<QWidget*> widgets;
	for (const auto& splitter : m_HSplitters) {
		for (auto i = 0; i < splitter->count(); ++i) {
			auto widget = splitter->widget(i);
			if (widget) widgets.push_back(widget);
		}
	}
	return widgets;
}

/**
* Get a vector with all AbstractPlot objects in this layout.
**/
std::vector<AbstractPlot*> FlexGridLayout::plots() const {
	std::vector<AbstractPlot*> plots;
	auto widgets = this->widgets();
	for (const auto& widget : widgets) {
		auto plot = qobject_cast<AbstractPlot*>(widget);
		if (plot != nullptr) plots.push_back(plot);
	}
	return plots;
}

/**
* Set layout to the given number of rows and columns.
* 
* @param rows Number of rows for new layout.
* @param cols Number of columns for new layout.
**/
void FlexGridLayout::setLayout(int rows, int cols) {

	// Re-parent the widgets to the parent widget, so they are not deleted when splitters are deleted.
	auto plots = this->plots();
	for (const auto& plot : plots) {
		plot->setParent(m_PlotManager);
	}

	// Delete all splitters
	m_HSplitters.clear();
	delete m_VSplitter;
	m_VSplitter = new QSplitter(Qt::Vertical, m_PlotManager);
	m_VSplitter->setChildrenCollapsible(false);
	this->layout()->addWidget(m_VSplitter);

	// Delete empty plots from memory
	auto plotIt = plots.begin();
	while (plotIt != plots.end()) {
		if ((*plotIt)->isEmpty()) {
			delete* plotIt;
			plotIt = plots.erase(plotIt);
		}
		else {
			plotIt++;
		}
	}

	// Create new layout and fill it with existing and/or new plots
	int plot_counter = 0;
	QList<int> sizesV, sizesH; // Used to reset the widths of plots to be equal
	for (int row_i = 0; row_i < rows; ++row_i)
	{
		QSplitter* HSplitter = new QSplitter(Qt::Horizontal);
		HSplitter->setChildrenCollapsible(false);
		m_HSplitters.push_back(HSplitter);
		sizesH.clear();
		for (uint8_t col_i = 0; col_i < cols; ++col_i)
		{
			sizesH.push_back(QGuiApplication::primaryScreen()->virtualSize().width());
			// Use existing plot if possible, otherwise make a new one
			if (plots.size() >= plot_counter + 1)
			{
				HSplitter->addWidget(plots[plot_counter]);
				plots[plot_counter]->show();
			}
			else {
				auto plot = m_PlotManager->createPlot();
				HSplitter->addWidget(plot);
			}
			++plot_counter;
		}

		HSplitter->setSizes(sizesH);
		m_VSplitter->addWidget(HSplitter);
		sizesV.push_back(QGuiApplication::primaryScreen()->virtualSize().height());
	}
	m_VSplitter->setSizes(sizesV);

	// Delete plots that don't fit new layout from memory
	if (plots.size() > rows * cols) {
		while ((rows * cols) > plots.size()) {
			auto plot = plots.back();
			plots.pop_back();
			plot->deleteLater();
		}
	}
}


/**
* Replace a given widget by a new widget.
* 
* @param old Widget to replace.
* @param new Widget to replace the old one with.
**/
QWidget* FlexGridLayout::replaceWidget(QWidget* oldWidget, QWidget* newWidget) {
	auto parent = qobject_cast<QSplitter*>(oldWidget->parent());
	if (!parent) {
		H2A::logWarning("Failed to replace widget");
		return oldWidget;
	}
	parent->replaceWidget(parent->indexOf(oldWidget), newWidget);
	oldWidget->deleteLater();
	return newWidget;
}

/**
* Insert a widget in the layout, relative to a given reference widget.
* 
* @param reference Reference widget.
* @param newWidget New widget to add in the layout.
* @param dir Direction relative to reference widget where new widget is placed.
**/
void FlexGridLayout::insertWidget(QWidget* reference, QWidget* newWidget, H2A::Direction dir) {
	// Find splitter in which the source widget lives
	QSplitter* refSplitter = qobject_cast<QSplitter*>(reference->parent());
	if (refSplitter == nullptr) {
		// Should be impossible, but to avoid errors.
		H2A::logWarning("Failed to insert widget");
		return;
	}

	// Add the plot
	QList<int> sizes;
	if (dir == H2A::Up || dir == H2A::Down) {
		QSplitter* splitter = new QSplitter(Qt::Horizontal);
		splitter->addWidget(newWidget);
		auto refSplitterIndex = std::find(m_HSplitters.begin(), m_HSplitters.end(), refSplitter);
		if (refSplitterIndex == m_HSplitters.end()) return;
		m_HSplitters.insert(refSplitterIndex, splitter);
		uint8_t offset = (dir == H2A::Up) ? 0 : 1;
		m_VSplitter->insertWidget(m_VSplitter->indexOf(refSplitter) + offset, splitter);
		for (int i = 0; i < m_VSplitter->count(); ++i) sizes.push_back(QGuiApplication::primaryScreen()->virtualSize().height());
		m_VSplitter->setSizes(sizes);
	}
	else {
		uint8_t offset = (dir == H2A::Left) ? 0 : 1;
		refSplitter->insertWidget(refSplitter->indexOf(reference) + offset, newWidget);
		for (int i = 0; i < refSplitter->count(); ++i) sizes.push_back(QGuiApplication::primaryScreen()->virtualSize().width());
		refSplitter->setSizes(sizes);
	}
}


