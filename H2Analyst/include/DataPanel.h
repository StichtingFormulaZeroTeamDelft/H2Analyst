#pragma once

#include <map>
#include <algorithm>
#include <vector>

#include <QWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QString>
#include <QMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QLineEdit>

#include "DataStore.h"
#include "DataStructures.h"
#include "TreeView.h"

namespace H2A
{
    enum class ItemType : uint8_t {
        kDatafile, kSystem, kSubsystem, kDataset,
    };

    namespace ItemRole {
        const uint32_t kItemType = Qt::UserRole + 1;
        const uint32_t kSorting = Qt::UserRole + 2;
        const uint32_t kDatasetPtr = Qt::UserRole + 3;
        const uint32_t kFilter = Qt::UserRole + 4;
    };
}

Q_DECLARE_METATYPE(const void*)
Q_DECLARE_METATYPE(H2A::ItemType)

class DataPanel :
    public QWidget
{

    Q_OBJECT

private:

    DataStore* m_DataStore;
    QVBoxLayout* m_Layout;
    QLineEdit* m_SearchBox;
    
    QStandardItemModel* m_TreeItemModel;
    QItemSelectionModel* m_TreeSelectionModel;
    QSortFilterProxyModel* m_TreeProxyModel;
    TreeView* m_TreeView;


    QPoint m_DragStartPosition;

    QStandardItem* createTreeItemFromDatafile(const H2A::Datafile* df);
    QStandardItem* createTreeItem(const H2A::ItemType& type, const std::string& name, const H2A::Dataset* ds = nullptr);
    void addChildrenDatasets(const QStandardItem* item, std::vector<const H2A::Dataset*>& target) const;

    const H2A::Dataset* getDatasetFromItem(const QStandardItem* item) const;
    void applyFindFilter();


public:
    DataPanel(QWidget* parent = nullptr);

    void setDataStore(DataStore* datastore);
    std::vector<const H2A::Dataset*> getSelectedDatasets() const;

    void requestDatasetPopulation(const H2A::Dataset* dataset, bool blocking = false) const;
    void requestDatasetPopulation(std::vector<const H2A::Dataset*> datasets, bool blocking = false) const;

private slots:
    void searchInputChanged();

public slots:
    void updateData();

};
