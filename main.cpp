#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(puzzle);

    QApplication a(argc, argv);
    MainWindow w;
    w.loadImage(QStringLiteral(":/images/example.jpg"));
    w.show();

    return a.exec();
}
