#pragma once

#include <QColor>

#include <vector>
#include <iostream>
#include <string>

namespace H2A
{
    /**
    * General
    **/
	enum Direction { Left, Right, Up, Down };

    // Logging
    inline void log(const std::string& mess) { std::cout << mess << std::endl; };
    inline void logInfo(const std::string& mess) { std::cout << "[INFO] " << mess << std::endl; };
    inline void logWarning(const std::string& mess) { std::cout << "[Warning] " << mess << std::endl; };

    /**
    * Plots
    **/
    enum PlotType { Abstract, Time, XY, EmcyList };

    const std::vector<QColor> PlotColors = {
        QColor(0, 114, 189),
        QColor(217, 83, 25),
        QColor(237, 177, 32),
        QColor(126, 47, 142),
        QColor(119, 172, 48),
        QColor(77, 190, 238),
        QColor(162, 20, 47),
    };


    /**
    * DataPanel
    **/
    enum class ItemType : uint8_t {
        Datafile, System, Subsystem, Dataset,
    };

    namespace ItemRole {
        const uint32_t ItemType = Qt::UserRole + 1;
        const uint32_t Sorting = Qt::UserRole + 2;
        const uint32_t DataPtr = Qt::UserRole + 3; // Used in DataPanel to store pointer to datafile/dataset
        const uint32_t Filter = Qt::UserRole + 4;
        const uint32_t Emcy = Qt::UserRole + 5; // Used in Emcy list to store emcy object
    };

    const int TREEVIEW_FILTER_EXPAND_THRESHOLD = 50;

    /**
    * ControlPanel
    **/
    enum Car { Forze8, Forze9 };
}