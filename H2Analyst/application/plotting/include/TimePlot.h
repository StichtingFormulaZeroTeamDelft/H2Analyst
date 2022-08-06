#pragma once

#include "AbstractPlot.h"
#include <set>
#include "TimeGraph.h"

/**
* Time-based plot, which extends the AbstractPlot class.
**/
class TimePlot : public AbstractPlot
{
	
	Q_OBJECT

	const float STD_VIEW_PADDING = 0.2f;
	const float LIMIT_PADDING = 1.0f;

public:

	TimePlot(QWidget* parent);

	// Actions
	void plot(std::vector<const H2A::Dataset*> datasets, bool clearFirst = false) override;
	void resetView() override;

protected:

	virtual void mouseDoubleClickEvent(QMouseEvent* event);
	virtual void setAxisLabels() override;

};


