#pragma once

#include <algorithm>
#include <boost/range/combine.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <fstream>
#include <filesystem>
#include <map>
#include <iomanip>

#include <QListWidget>
#include <QToolButton>
#include <QVariant>

#include "Emcies.h"
#include "AbstractPlot.h"
#include "DataPanel.h"
#include "StringOperations.h"


/**
* Emcy codes defined here: https://github.com/StichtingFormulaZeroTeamDelft/Software_F8/blob/fda9a0777535e28049107f6815ac82edf8b60bf5/iar_workspace/generic/definitions/emcy_list.c
**/

Q_DECLARE_METATYPE(H2A::Emcy::Emcy)

class TimeCursorItem : public QStandardItem
{
public:
	TimeCursorItem() : QStandardItem("") {};
};

class EmcyFilterModel : public QSortFilterProxyModel
{
public:
	EmcyFilterModel(QObject* parent = nullptr);
	void updateFilter(H2A::Emcy::Severity severity, bool checked);
	void updateFilter(H2A::Emcy::Level level, bool checked);
	bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
private:
	uint8_t m_SeverityFilterState;
	uint16_t m_LevelFilterState; 
};

class ItemDelegate : public QStyledItemDelegate
{
	EmcyFilterModel* m_Model;
	TimeCursorItem* m_TimeCursor;
public:
	ItemDelegate(EmcyFilterModel* model, TimeCursorItem* timeCursor, QObject* parent = nullptr);
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};


class EmcyPlot : public AbstractPlot
{
	Q_OBJECT

	const int WARNING_ICON_WIDTH = 24;

	QVBoxLayout* m_VLayout;
	QWidget* m_WarningMsg;

	QToolButton* m_TbFilter;

	// List view objects
	QListView* m_List;
	QStandardItemModel* m_ListModel;
	ItemDelegate* m_ItemDelegate;
	EmcyFilterModel* m_EmcyFilterModel;

	const DataPanel* m_DataPanel;

	QMutex* m_DataMutex;
	std::vector<const H2A::Dataset*> m_EmcyDatasets;
	std::map<uint16_t, H2A::Emcy::Properties> m_EmcyProperties;
	
	// Time cursor
	TimeCursorItem* m_TimeCursor;

	bool getDatasets();
	void fillList();
	void addItem(const H2A::Emcy::Emcy& emcy);
	void sort();
	//void drawTimeCursor();
	QMenu* buildFilterMenu();

public:
	EmcyPlot(const DataPanel* dataPanel, H2A::Car car, QWidget* parent = nullptr);

	bool isEmpty() const override { return false; };

protected:
	virtual void resizeEvent(QResizeEvent* event);

private slots:
	void itemDoubleClicked(const QModelIndex&);

public slots:
	virtual void setTimeCursorEnabled(bool enable);
	virtual void setTimeCursorTime(double time);
	void setSelectedCar(H2A::Car car);

signals:
	void setTimeCursor(double time);
};

