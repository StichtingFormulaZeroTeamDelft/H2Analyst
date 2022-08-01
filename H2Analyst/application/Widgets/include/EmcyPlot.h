#pragma once


#include <QListWidget>
#include "Emcies.h"
#include "AbstractPlot.h"
#include "DataPanel.h"
#include "StringOperations.h"

#include <QVariant>

#include <algorithm>
#include <boost/range/combine.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <fstream>
#include <filesystem>
#include <map>
#include <iomanip>

/**
* Emcy codes defined here: https://github.com/StichtingFormulaZeroTeamDelft/Software_F8/blob/fda9a0777535e28049107f6815ac82edf8b60bf5/iar_workspace/generic/definitions/emcy_list.c
**/

Q_DECLARE_METATYPE(H2A::Emcy::Emcy)

class TimeCursorItem : public QStandardItem
{
public:
	TimeCursorItem() : QStandardItem("") {};
};

class ItemDelegate : public QStyledItemDelegate
{
	QStandardItemModel* m_Model;
	TimeCursorItem* m_TimeCursor;
public:
	ItemDelegate(QStandardItemModel* model, TimeCursorItem* timeCursor, QObject* parent = nullptr);
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};


class EmcyPlot : public AbstractPlot
{
	Q_OBJECT

	// List view objects
	QListView* m_List;
	QStandardItemModel* m_ListModel;
	ItemDelegate* m_ItemDelegate;

	const DataPanel* m_DataPanel;

	std::map<uint16_t, H2A::Emcy::Properties> m_EmcyProperties;
	const std::map < uint32_t, std::string> m_EmcyUIDs = {};

	// Time cursor
	TimeCursorItem* m_TimeCursor;
	int m_TimeCursorRow;

	void fillList();
	void addItem(const H2A::Emcy::Emcy& emcy);
	void sort();
	//void drawTimeCursor();

public:
	EmcyPlot(const DataPanel* dataPanel, QWidget* parent = nullptr);

	bool isEmpty() const override { return false; };

protected:
	virtual void resizeEvent(QResizeEvent* event);

private slots:
	void itemDoubleClicked(const QModelIndex&);

public slots:
	virtual void setTimeCursorEnabled(bool enable);
	virtual void setTimeCursorTime(double time);

signals:
	void setTimeCursor(double time);
};

