#include <QCoreApplication>
#include "Server_test.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server_test test;
    return a.exec();
}
