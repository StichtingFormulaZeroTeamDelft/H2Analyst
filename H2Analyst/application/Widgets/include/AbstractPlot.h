#pragma once

#include "qcustomplot.h"

/**
* Base class for any plot widget.
**/
class AbstractPlot : public QCustomPlot
{

	Q_OBJECT

public:

	AbstractPlot(QWidget* parent = nullptr);


};