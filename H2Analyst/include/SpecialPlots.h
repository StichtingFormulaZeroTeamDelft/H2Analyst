#pragma once

#include "DataStructures.h"
#include <vector>
#include <string>
#include <algorithm>
#include "PlotDefinitions.h"

const bool isSpecial(const std::vector<const H2A::Dataset*> datasets, const H2A::PlotType type, H2A::specialPlot& plot);
void orderDatasets(std::vector<const H2A::Dataset*>& datasets, const H2A::specialPlot plot);