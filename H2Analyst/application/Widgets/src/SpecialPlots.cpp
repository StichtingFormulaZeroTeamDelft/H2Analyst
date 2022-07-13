#include "SpecialPlots.h"


/**
* Function to check whether the supplied list of datasets and plot type form a special plot together.
*
* @param datasets List of datasets that will be plotted.
* @param type Type of plot.
* @param plot Object to store the special plot struct in case it matches.
**/
const bool isSpecial(const std::vector<const H2A::Dataset*> datasets, const H2A::PlotType type, H2A::specialPlot& plot) {

    // Check if list of datasets matches any of the defined special plots
    std::vector<uint32_t> ds_uids;
    for (const auto& dataset : datasets)
        ds_uids.push_back(dataset->uid);
    std::sort(ds_uids.begin(), ds_uids.end());

    for (const auto& specialPlot : H2A::SpecialPlots) {
        if (type != specialPlot.type) continue;

        std::vector<uint32_t> uids({ specialPlot.x, specialPlot.y });
        std::sort(uids.begin(), uids.end());
        if (ds_uids == uids) {
            plot = specialPlot;
            return true;
        }
    }
    return false;

}

/**
* Function that makes sure the datasets are in the correct order for plotting the special plot.
* This function assumes the vector of datasets has already been confirmed to be special with the isSpecial() function.
* 
* @param datasets Vector of datasets to order.
* @param plot SpecialPlot that is being plotted.
**/
void orderDatasets(std::vector<const H2A::Dataset*>& datasets, const H2A::specialPlot plot)
{
    switch (plot.type)
    {
    case H2A::Time:
    {
        break;
    }
    case H2A::XY:
    {
        if (datasets[0]->uid != plot.x)
            std::iter_swap(datasets.begin(), datasets.begin()+1);
        break;
    }
    default:
        break;
    }
}
