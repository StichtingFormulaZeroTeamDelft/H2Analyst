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

    DataStore* m_DataStore;
    QVBoxLayout* m_Layout;
    QLineEdit* m_SearchBox;
    
    QStandardItemModel* m_TreeItemModel;
    QItemSelectionModel* m_TreeSelectionModel;
    QSortFilterProxyModel* m_TreeProxyModel;
    TreeView* m_TreeView;


    QPoint m_DragStartPosition;

    QStandardItem* createTreeItemFromDatafile(const H2A::Datafile* df);
    QStandardItem* createTreeItem(const H2A::Datafile* datafile, const std::string& name);
    QStandardItem* createTreeItem(const H2A::Dataset* dataset, const std::string& name);
    QStandardItem* createTreeItem(const H2A::ItemType& type, const std::string& name);
    void addChildrenDatasets(const QStandardItem* item, std::vector<const H2A::Dataset*>& target) const;

    const H2A::Dataset* getDatasetFromItem(const QStandardItem* item) const;
    void applyFindFilter();
    int DataPanel::countRows(const QModelIndex& index = QModelIndex());


public:
    DataPanel(QWidget* parent = nullptr);

    void setDataStore(DataStore* datastore) { m_DataStore = datastore; };
    bool datasetPresentUID(const H2A::Datafile* file, const uint32_t uid) const;
    const std::vector<H2A::Datafile*> getDatafiles() const { return m_DataStore->getDatafiles(); };
    std::vector<const H2A::Dataset*> getSelectedDatasets() const;
    std::vector<const H2A::Datafile*> getSelectedDatafiles() const;

    void requestDatasetPopulation(const H2A::Dataset* dataset, bool blocking = false) const;
    void requestDatasetPopulation(std::vector<const H2A::Dataset*> datasets, bool blocking = false) const;

private slots:
    void searchInputChanged();

public slots:
    void updateData();

};
