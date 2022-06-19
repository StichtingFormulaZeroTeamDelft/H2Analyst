#pragma once


namespace H2A {

    enum PlotType {
        Time, XY
    };


    const struct specialPlot {
        std::string name = "";
        H2A::PlotType type = H2A::PlotType::Time;
        uint32_t x = 0;
        uint32_t y = 0;
    };

    const std::vector<specialPlot> SpecialPlots{
        specialPlot{"GPS track", H2A::PlotType::XY, 28681, 28680},
        specialPlot{"Compressor map", H2A::PlotType::XY, 0, 1}
    };

}
