#include "PlotManager.h"

PlotManager::PlotManager(QWidget* parent) : QWidget(parent),
m_Layout(new FlexGridLayout(this)),
m_DataPanel(nullptr),
m_TimeAlignEnabled(true),
m_BusyAligning(false),
m_TimeCursorEnabled(false),
m_TimeCursorTime(0.0)
{
}

/**
* Convenience function for creating a new plot, setting its members and connecting signals.
**/
AbstractPlot* PlotManager::createPlot(H2A::PlotType type) {
	AbstractPlot* plot = new AbstractPlot(this);
	switch (type)
	{
	case H2A::Time:
		plot = new TimePlot(this);
		break;
	case H2A::XY:
		break;
	case H2A::EmcyList:
		plot = new EmcyPlot(m_DataPanel, this);
		break;
	default:
		break;
	}
	connect(plot, SIGNAL(timeAxisChanged(AbstractPlot*)), this, SLOT(alignTimeAxis(AbstractPlot*)));
	connect(plot, &AbstractPlot::plotSelected, this, &PlotManager::plotSelected);
	connect(plot, &AbstractPlot::contextMenuRequested, this, &PlotManager::contextMenu);

	plot->setTimeCursorEnabled(m_TimeCursorEnabled);
	plot->setTimeCursorTime(m_TimeCursorTime);
	connect(plot, &AbstractPlot::timeCursorPlaced, this, [=]() {this->setTimeCursorEnabled(true); });
	connect(plot, &AbstractPlot::timeCursorPlaced, this, &PlotManager::setTimeCursorTime);
	
	return plot;
}

/**
* Function that creates and executes the dialog for setting the layout.
**/
void PlotManager::setPlotLayoutDialog() {
	DialogPlotLayout dialog(this);
	dialog.exec();
	if (dialog.result() == QDialog::Accepted)
		this->setPlotLayout(dialog.m_SelectedLayout.front(), dialog.m_SelectedLayout.back());
}

/**
* Function that sets the layout of plots to a given number of rows and colums.
*
* @param rows Number of rows in new layout.
* @param cols Number of cols in new layout.
**/
void PlotManager::setPlotLayout(uint8_t rows, uint8_t cols) {
	m_Layout->setLayout(rows, cols);
}

/**
* Slot that can be called to align plots to match the given reference plot.
* If no reference is supplied, align plots to match the one with currently the smallest data range.
* 
* @param reference Plot to align time axis of all plots to (dafault = nullptr).
**/
void PlotManager::alignTimeAxis(AbstractPlot* ref) {
	
	if (!m_TimeAlignEnabled) return;

	// Avoid aligning time axis when a plot is cleared
	if (ref != nullptr) { if (ref->isEmpty()) return; }

	if (!m_BusyAligning) {
		// Setting ranges of other plots causes this slots to be called recursively.
		// m_BusyAligning is used to ignore recursive calls to this function.
		m_BusyAligning = true;

		// If no ref is given, find the plot with the smallest time axis range and use that to set the others.
		auto plots = this->plots();
		if (ref == nullptr) {
			QCPRange range(QCPRange::minRange, QCPRange::maxRange);
			for (const auto& plot : plots) {
				if (plot->type() == H2A::Time && plot->xAxis->range().size() < range.size()) {
					range = plot->xAxis->range();
					ref = plot;
				}
			}
			if (ref == nullptr) ref = plots.front(); // Should never be necessary, but to avoid errors.
		}

		// Set plots to match the reference.
		for (const auto& plot : plots) {
			if (!plot->type() == H2A::Time || plot == ref) continue;
			plot->xAxis->setRange(ref->xAxis->range());
			//plot->zoomYToData();
			plot->replot();
		}

		m_BusyAligning = false;
	}
}

/**
* Slot that enabled time axis alignment between the plots in this manager.
*
* @param align Enable time axis alignment.
**/
void PlotManager::setTimeAlignEnabled(bool align) {
	m_TimeAlignEnabled = align;
	if (m_TimeAlignEnabled) this->alignTimeAxis();

}

/**
* Function that reset the views of all plots in this manager.
**/
void PlotManager::resetAllViews() {
	for (const auto& plot : this->plots())
		if (!plot->isEmpty()) plot->resetView();
}

/**
* Function that returns true if all plots in this manager are empty.
**/
bool PlotManager::allPlotsEmpty() {
	for (const auto& plot : this->plots())
		if (!plot->isEmpty()) return false;
	return true;
}

/**
* Function to set the time of the time cursors of the plots in this manager.
*
* @param time Time to set the time cursor to.
**/
void PlotManager::setTimeCursorTime(double time) {
	m_TimeCursorTime = time;
	for (const auto& plot : this->plots()) plot->setTimeCursorTime(time);
	emit this->timeCursorMoved(m_TimeCursorTime);
}

/**
* Function to enable or disable the time cursors in the plots of this manager.
*
* @param enabled Enable time cursors.
**/
void PlotManager::setTimeCursorEnabled(bool enabled) {
	m_TimeCursorEnabled = enabled;
	for (const auto& plot : this->plots())
		plot->setTimeCursorEnabled(enabled);
}

/**
* When this slot is called, it removes the given widget from the layout and deletes it.
**/
void PlotManager::deletePlot(AbstractPlot* source) {
	if (source == nullptr) return;

	// Check if source is not the last plot.
	if (this->plots().size() < 2) {
		H2A::Dialog::message("Plotting data becomes quite hard without a single plot...");
		return;
	}

	// Hide plot so layout can adapt and delete it from memory.
	source->hide();
	source->deleteLater();
}

/**
* This slot can be called to add plot(s) relative to an existing one.
* 
* @param ref Reference plot.
* @param dir Direction to add the plot, relative to the reference plot.
**/
void PlotManager::insertPlot(AbstractPlot* ref, H2A::Direction dir) {
	auto plot = this->createPlot();
	m_Layout->insertWidget(ref, plot, dir);
}

/**
* Replaces an existing plot with a newly created plot of given type.
* 
* @param source Plot to replace.
* @param newType Type of new plot.
**/
AbstractPlot* PlotManager::replacePlot(AbstractPlot* source, H2A::PlotType newType) {
	AbstractPlot* newPlot = createPlot(newType);
	return qobject_cast<AbstractPlot*>(m_Layout->replaceWidget(source, newPlot));
}

/**
* When this slot is called, the currently selected datasets are plotted in the target plot.
* If no plot is specified, it selects the first one that is available.
* 
* @param target Plot widget to plot selected data in.
* @param type Type of plot requested
**/
void PlotManager::plotSelected(AbstractPlot* target, H2A::PlotType type, bool clearFirst) {

	// Get selected datasets and make sure they are populated
	auto datasets = m_DataPanel->getSelectedDatasets();
	m_DataPanel->requestDatasetPopulation(datasets, true);

	// No target specified, find the first one that is still empty
	if (!target) {
		for (const auto& plot : this->plots()) {
			if (plot->isEmpty()) {
				target = plot;
				break;
			}
		}
		// If no empty plot is found, abort plotting.
		if (!target) {
			H2A::Dialog::message("No empty plots available for plotting.");
			return;
		}
	}

	// Check if target is right type. If not, change it. Then plot.
	target = target->type() == type ? target : this->replacePlot(target, type);
	target->plot(datasets, clearFirst);
}

/**
* Function that creates the abstract context menu that applies to all plots.
**/
void PlotManager::contextMenu(AbstractPlot* source, const QPoint& pos) {

	QMenu menu(this);
	bool enabled;

	QAction* acPlot = new QAction("Plot");
	enabled = m_DataPanel->getSelectedDatasets().size() > 0;
	acPlot->setEnabled(enabled);
	connect(acPlot, &QAction::triggered, [=]() { this->plotSelected(source, H2A::Time); });
	menu.addAction(acPlot);

	QMenu* plotMenu = menu.addMenu(QIcon(QPixmap(":/icons/more-information")), "Other plots");

	QAction* acPlotXY = new QAction("XY");
	acPlotXY->setEnabled(false);
	// connect action
	plotMenu->addAction(acPlotXY);

	QAction* acPlotEmcy = new QAction("Emcy");
	enabled = m_DataPanel->getSelectedDatafiles().size() == 1 || m_DataPanel->getDatafiles().size() == 1;
	acPlotEmcy->setEnabled(enabled);
	connect(acPlotEmcy, &QAction::triggered, [=]() {this->replacePlot(source, H2A::EmcyList); });
	plotMenu->addAction(acPlotEmcy);

	QAction* acResetView = new QAction(QIcon(QPixmap(":/icons/uno")), QString("Reset view"));
	acResetView->setEnabled(!source->isEmpty() && source->type() != H2A::EmcyList);
	connect(acResetView, &QAction::triggered, [=]() {source->resetView(); });
	menu.addAction(acResetView);

	QAction* acResetAllViews = new QAction(QIcon(QPixmap(":/icons/uno")), QString("Reset all views"));
	connect(acResetAllViews, &QAction::triggered, [=]() {this->resetAllViews(); });
	menu.addAction(acResetAllViews);

	QAction* acClear = new QAction(QString("Clear"));
	acClear->setEnabled(!source->isEmpty() && source->type() != H2A::EmcyList);
	connect(acClear, &QAction::triggered, [=]() {source->clear(); });
	menu.addAction(acClear);

	QAction* acDelete = new QAction(QIcon(QPixmap(":/icons/remove")), QString("Delete plot"));
	connect(acDelete, &QAction::triggered, [=]() {this->deletePlot(source); });
	menu.addAction(acDelete);

	QMenu* insertMenu = menu.addMenu(QIcon(QPixmap(":/icons/plus")), "Insert plot");

	QAction* acInsertLeft = new QAction(QIcon(QPixmap(":/icons/left-arrow")), QString("Insert left"));
	connect(acInsertLeft, &QAction::triggered, [=]() {this->insertPlot(source, H2A::Left); });
	insertMenu->addAction(acInsertLeft);

	QAction* acInsertRight = new QAction(QIcon(QPixmap(":/icons/right-arrow")), QString("Insert right"));
	connect(acInsertRight, &QAction::triggered, [=]() {this->insertPlot(source, H2A::Right); });
	insertMenu->addAction(acInsertRight);

	QAction* acInsertAbove = new QAction(QIcon(QPixmap(":/icons/up-arrow")), QString("Insert above"));
	connect(acInsertAbove, &QAction::triggered, [=]() {this->insertPlot(source, H2A::Up); });
	insertMenu->addAction(acInsertAbove);

	QAction* acInsertBelow = new QAction(QIcon(QPixmap(":/icons/down-arrow")), QString("Insert below"));
	connect(acInsertBelow, &QAction::triggered, [=]() {this->insertPlot(source, H2A::Down); });
	insertMenu->addAction(acInsertBelow);

	QAction* acClip = new QAction(QIcon(QPixmap(":/icons/screenshot")), QString("Screenshot"));
	connect(acClip, &QAction::triggered, source, &AbstractPlot::clip);
	menu.addAction(acClip);

	menu.exec(source->mapToGlobal(pos));
}