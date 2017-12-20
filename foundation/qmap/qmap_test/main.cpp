#include <QCoreApplication>
#include"map.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MapTest c;
    c.showMap();
    return a.exec();
}
