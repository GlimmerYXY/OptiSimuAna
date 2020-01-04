#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QPoint>
#include <QPixmap>
#include <QVector>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
QT_END_NAMESPACE

class CanvasWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CanvasWidget(int imageSize, QWidget *parent = nullptr);
    void clear();

    int pieceSize() const;
    int imageSize() const;

signals:
    void puzzleCompleted();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    struct Piece {
        QPixmap pixmap;
        QRect rect;
        QPoint location;
    };

    int findPiece(const QRect &pieceRect) const;
    const QRect targetSquare(const QPoint &position) const;

    QVector<Piece> pieces;
    QRect highlightedRect;
    int inPlace;
    int m_ImageSize;
};

#endif // CANVASWIDGET_H
