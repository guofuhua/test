#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>

#include "screenshot.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
    Screenshot screenshot;
    screenshot.move(QApplication::desktop()->availableGeometry(&screenshot).topLeft() + QPoint(1200, 200));
    screenshot.show();
    return a.exec();
}
