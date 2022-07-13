#pragma once

#include <iostream>

#include <QTreeView>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>

/*

Subclass of QTreeView to allow the implementation
of some custom behaviour like dragging/dropping

*/

class TreeView :
    public QTreeView
{

    QPoint m_DragStartPosition;

public:

    TreeView(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

};

