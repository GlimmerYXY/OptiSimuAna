#include "canvaswidget.h"
#include "devicelist.h"

#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QPainter>

CanvasWidget::CanvasWidget(int imageSize, QWidget *parent)
    : QWidget(parent), m_ImageSize(imageSize)
{
    setAcceptDrops(true);
    setMinimumSize(m_ImageSize, m_ImageSize);
    setMaximumSize(m_ImageSize, m_ImageSize);
}

void CanvasWidget::clear()
{
    pieces.clear();
    highlightedRect = QRect();
    inPlace = 0;
    update();
}

void CanvasWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(DeviceList::libMimeType()))
        event->accept();
    else
        event->ignore();
}

void CanvasWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    QRect updateRect = highlightedRect;
    highlightedRect = QRect();
    update(updateRect);
    event->accept();
}

void CanvasWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QRect updateRect = highlightedRect.united(targetSquare(event->pos()));

    if (event->mimeData()->hasFormat(DeviceList::libMimeType())
        && findPiece(targetSquare(event->pos())) == -1) {

        highlightedRect = targetSquare(event->pos());
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else {
        highlightedRect = QRect();
        event->ignore();
    }

    update(updateRect);
}

void CanvasWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(DeviceList::libMimeType())
        && findPiece(targetSquare(event->pos())) == -1) {

        QByteArray pieceData = event->mimeData()->data(DeviceList::libMimeType());
        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
        Piece piece;
        piece.rect = targetSquare(event->pos());
        dataStream >> piece.pixmap >> piece.location;

        pieces.append(piece);

        highlightedRect = QRect();
        update(piece.rect);

        event->setDropAction(Qt::MoveAction);
        event->accept();

        if (piece.location == piece.rect.topLeft() / pieceSize()) {
            inPlace++;
            if (inPlace == 25)
                emit puzzleCompleted();
        }
    } else {
        highlightedRect = QRect();
        event->ignore();
    }
}

int CanvasWidget::findPiece(const QRect &pieceRect) const
{
    for (int i = 0, size = pieces.size(); i < size; ++i) {
        if (pieces.at(i).rect == pieceRect)
            return i;
    }
    return -1;
}

void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    QRect square = targetSquare(event->pos());
    const int found = findPiece(square);

    if (found == -1)
        return;

    Piece piece = pieces.takeAt(found);

    if (piece.location == square.topLeft() / pieceSize())
        inPlace--;

    update(square);

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);

    dataStream << piece.pixmap << piece.location;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(DeviceList::libMimeType(), itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(event->pos() - square.topLeft());
    drag->setPixmap(piece.pixmap);

    if (drag->exec(Qt::MoveAction) != Qt::MoveAction) {
        pieces.insert(found, piece);
        update(targetSquare(event->pos()));

        if (piece.location == square.topLeft() / pieceSize())
            inPlace++;
    }
}

void CanvasWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(event->rect(), Qt::white);

    if (highlightedRect.isValid()) {
        painter.setBrush(QColor("#ffcccc"));
        painter.setPen(Qt::NoPen);
        painter.drawRect(highlightedRect.adjusted(0, 0, -1, -1));
    }

    for (const Piece &piece : pieces)
        painter.drawPixmap(piece.rect, piece.pixmap);
}

const QRect CanvasWidget::targetSquare(const QPoint &position) const
{
    return QRect(position / pieceSize() * pieceSize(),
                 QSize(pieceSize(), pieceSize()));
}

int CanvasWidget::pieceSize() const
{
    return m_ImageSize / 5;
}

int CanvasWidget::imageSize() const
{
    return m_ImageSize;
}
