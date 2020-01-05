#include "devicelist.h"

#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QMessageBox>

DeviceList::DeviceList(int pieceSize, QWidget *parent)
    : QListWidget(parent), m_DeviceSize(pieceSize)
{
    setDragEnabled(true);
    setViewMode(QListView::IconMode);
    setIconSize(QSize(m_DeviceSize, m_DeviceSize));
    setSpacing(10);
    setAcceptDrops(false);  //必须显示设置为false，即使注释掉也相当于true
    setDropIndicatorShown(true);
}

void DeviceList::startDrag(Qt::DropActions )
{
    QListWidgetItem *item = currentItem();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    QPixmap pixmap = qvariant_cast<QPixmap>(item->data(Qt::UserRole));
    QPoint location = item->data(Qt::UserRole+1).toPoint();

    dataStream << pixmap << location;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(DeviceList::libMimeType(), itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
    drag->setPixmap(pixmap);

    if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
        //delete takeItem(row(item));
        return;
}

/*AcceptDrops为true时有效——1*/
void DeviceList::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(DeviceList::libMimeType()))
        event->accept();
    else
        event->ignore();
}

/*AcceptDrops为true时有效——2*/
void DeviceList::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(DeviceList::libMimeType())) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else {
        event->ignore();
    }
}

/*AcceptDrops为true时有效——3*/
void DeviceList::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(DeviceList::libMimeType())) {
        QByteArray pieceData = event->mimeData()->data(DeviceList::libMimeType());
        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
        QPixmap pixmap;
        QPoint location;
        dataStream >> pixmap >> location;

        addDevice(pixmap, location);

        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else {
        event->ignore();
    }
}

void DeviceList::addDevice(const QPixmap &pixmap, const QPoint &location)
{
    QListWidgetItem *pieceItem = new QListWidgetItem(this);
    pieceItem->setIcon(QIcon(pixmap));
    pieceItem->setData(Qt::UserRole, QVariant(pixmap));
    pieceItem->setData(Qt::UserRole+1, location);
    pieceItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
}
