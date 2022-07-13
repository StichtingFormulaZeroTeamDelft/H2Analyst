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
#include "Namespace.h"

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
    int DataPanel::countRows(const QModelIndex& index = QModelIndex());


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
