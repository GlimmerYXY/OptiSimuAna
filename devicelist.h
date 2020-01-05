#ifndef DEVICELIST_H
#define DEVICELIST_H

#include <QListWidget>

class DeviceList : public QListWidget
{
    Q_OBJECT

public:
    explicit DeviceList(int deviceSize, QWidget *parent = nullptr);
    void addDevice(const QPixmap &pixmap, const QPoint &location);

    static QString libMimeType() { return QStringLiteral("image/x-lib-device"); }

protected:
    void startDrag(Qt::DropActions supportedActions) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    int m_DeviceSize;
};

#endif // DEVICELIST_H
