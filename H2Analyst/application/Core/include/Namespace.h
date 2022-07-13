#pragma once

#include <QColor>

#include <vector>

namespace H2A
{
    /**
    * General
    **/
	enum Direction { left, right, up, down };

    /**
    * Plots
    **/
    enum PlotType { Abstract, Time, XY };

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
        kDatafile, kSystem, kSubsystem, kDataset,
    };

    namespace ItemRole {
        const uint32_t kItemType = Qt::UserRole + 1;
        const uint32_t kSorting = Qt::UserRole + 2;
        const uint32_t kDatasetPtr = Qt::UserRole + 3;
        const uint32_t kFilter = Qt::UserRole + 4;
    };

    const int TREEVIEW_FILTER_EXPAND_THRESHOLD = 50;
}