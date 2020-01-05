#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextStream>
#include <QLabel>
#include <QtWidgets>

#include "devicelist.h"
#include "canvaswidget.h"
#include <stdlib.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //QLabel *label = new QLabel(ui->scrollAreaWidgetContents);
    //label->setObjectName(QString::fromUtf8("label"));
    //label->setGeometry(QRect(30, 40, 81, 61));
    //label->setPixmap(QPixmap(QString::fromUtf8("D:/Qt/Qt5.12.3/Examples/Qt-5.12.3/widgets/draganddrop/draggableicons/images/boat.png")));
    //label->move(10, 10);
    //label->show();
    //label->setAttribute(Qt::WA_DeleteOnClose);

    ui->dockProject->setGeometry(0, 46, 200, 330);
    ui->dockLibrary->setGeometry(0, 377, 200, 330);

    canvasWidget = new CanvasWidget;
    ui->tabCanvas->insertTab(0, canvasWidget, "画布");
    ui->tabCanvas->setCurrentIndex(0);

    deviceList = new DeviceList(80, ui->widLibrary);
    deviceList->setGeometry(0, 0, 200, 300);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_acOpen_triggered()
{//选择单个文件
    QString curPath = QDir::currentPath();//获取系统当前目录
    QString dlgTitle = "选择一个文件"; //对话框标题
    QString filter = "文本文件(*.txt);;图片文件(*.jpg *.gif *.png);;所有文件(*.*)"; //文件过滤器

    QString aFileName = QFileDialog::getOpenFileName(this, dlgTitle, curPath, filter);

    openTextByStream(aFileName);
}

bool MainWindow::openTextByIODevice(const QString &aFileName)
{//用IODevice方式打开文本文件
    QFile aFile(aFileName);
    if (! aFile.exists())
        return false;
    if (! aFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    ui->plainTextEdit->setPlainText(aFile.readAll());

    aFile.close();
    return  true;
}

bool MainWindow::openTextByStream(const QString &aFileName)
{ //用 QTextStream打开文本文件
    QFile aFile(aFileName);
    if (! aFile.exists())
        return false;
    if (! aFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream aStream(&aFile);
    ui->plainTextEdit->setPlainText(aStream.readAll());

    aFile.close();
    return true;
}

void MainWindow::on_acSave_triggered()
{//保存文件
    QString curPath=QCoreApplication::applicationDirPath(); //获取应用程序的路径
    QString dlgTitle="保存文件"; //对话框标题
    QString filter="文本文件(*.txt);;h文件(*.h);;C++文件(.cpp);;所有文件(*.*)"; //文件过滤器

    QString aFileName=QFileDialog::getSaveFileName(this,dlgTitle,curPath,filter);

    saveTextByStream(aFileName);
}

bool MainWindow::saveTextByIODevice(const QString &aFileName)
{//用IODevice方式保存文本文件
    QFile aFile(aFileName);
    if (! aFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QString str = ui->plainTextEdit->toPlainText();//整个内容作为字符串
    QByteArray strBytes = str.toUtf8();//转换为字节数组//QByteArray  strBytes=str.toLocal8Bit();
    aFile.write(strBytes, strBytes.length());  //写入文件

    aFile.close();
    return true;
}

bool MainWindow::saveTextByStream(const QString &aFileName)
{//用QTextStream保存文本文件
    QFile aFile(aFileName);
    if (! aFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream aStream(&aFile); //用文本流读取文件
    QString str = ui->plainTextEdit->toPlainText(); //转换为字符串
    aStream << str; //写入文本流

    aFile.close();//关闭文件
    return true;
}

void MainWindow::on_acNew_triggered()
{
    int count = ui->tabCanvas->count();

    QString str("画布" + QString::number(count+1));
    //QLabel *label = new QLabel(str);
    CanvasWidget * testWidget = new CanvasWidget;

    canvasWidget = new CanvasWidget;
    ui->tabCanvas->insertTab(0, canvasWidget, "画布");
    ui->tabCanvas->setCurrentIndex(0);

    ui->tabCanvas->insertTab(0, testWidget, str);
    ui->tabCanvas->setCurrentIndex(0);
}


void MainWindow::openImage()
{
    const QString directory =
        QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).value(0, QDir::homePath());
    QFileDialog dialog(this, tr("Open Image"), directory);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    QStringList mimeTypeFilters;
    for (const QByteArray &mimeTypeName : QImageReader::supportedMimeTypes())
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (dialog.exec() == QDialog::Accepted)
        loadImage(dialog.selectedFiles().constFirst());
}

void MainWindow::loadImage(const QString &fileName)
{
    QPixmap newImage;
    if (!newImage.load(fileName)) {
        QMessageBox::warning(this, tr("Open Image"),
                             tr("The image file could not be loaded."),
                             QMessageBox::Close);
        return;
    }
    puzzleImage = newImage;
    setupPuzzle();
}

void MainWindow::setupPuzzle()
{
    int width = puzzleImage.width();
    int height = puzzleImage.height();
    int size = qMin(width, height);
    int a = (width - size) / 2;
    int b = (height - size) / 2;
    int width2 = canvasWidget->width();
    int height2 = canvasWidget->height();
    puzzleImage = puzzleImage.copy(a, b, size, size).scaled(
                width2, height2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    deviceList->clear();

    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            int pieceSize = 400 / 5;
            QPixmap pieceImage = puzzleImage.copy(x * pieceSize, y * pieceSize, pieceSize, pieceSize);
            deviceList->addDevice(pieceImage, QPoint(x, y));
        }
    }

    for (int i = 0; i < deviceList->count(); ++i) {
        if (QRandomGenerator::global()->bounded(2) == 1) {
            QListWidgetItem *item = deviceList->takeItem(i);
            deviceList->insertItem(0, item);
        }
    }

    canvasWidget->update();
}
