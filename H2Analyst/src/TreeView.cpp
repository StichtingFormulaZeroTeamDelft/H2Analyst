#include "TreeView.h"

TreeView::TreeView(QWidget* parent) : QTreeView(parent),
	m_DragStartPosition()
{

}

void TreeView::mousePressEvent(QMouseEvent* event)
{
	// Save location to check later if user is dragging
	if (event->button() == Qt::LeftButton)
	{
		m_DragStartPosition = event->pos();
	}
	
	// Propagate event to QTreeView for other behaviour
	QTreeView::mousePressEvent(event);
}

void TreeView::mouseMoveEvent(QMouseEvent* event)
{
	if (!(event->buttons() & Qt::LeftButton))
		return;
	if ((event->pos() - m_DragStartPosition).manhattanLength() < QApplication::startDragDistance())
		return;

	QDrag* drag = new QDrag(this);
	QMimeData* mimeData = new QMimeData;

	//mimeData->setData(mimeType, data);
	drag->setMimeData(mimeData);

	Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);

	event->accept(); // By accepting, the event is not propagated to QTreeView to disable item selection by dragging
}
