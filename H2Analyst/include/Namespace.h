#pragma once

namespace H2A
{
	enum Direction { left, right, up, down };

    // ****** Datapanel ******
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