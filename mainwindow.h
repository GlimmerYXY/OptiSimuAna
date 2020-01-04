#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>

class DeviceList;
class CanvasWidget;
QT_BEGIN_NAMESPACE
class QListWidgetItem;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void loadImage(const QString &path);

public slots:
    void openImage();
    void setupPuzzle();

private slots:
    void on_acOpen_triggered();
    void on_acSave_triggered();
    void on_acNew_triggered();

private:
    Ui::MainWindow *ui;
    bool openTextByIODevice(const QString &aFileName);
    bool openTextByStream(const QString &aFileName);
    bool saveTextByIODevice(const QString &aFileName);
    bool saveTextByStream(const QString &aFileName);

    QPixmap puzzleImage;
    DeviceList *deviceList;
    CanvasWidget *canvasWidget;
};

#endif // MAINWINDOW_H
