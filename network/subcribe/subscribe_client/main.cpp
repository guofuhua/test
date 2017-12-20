#include <QCoreApplication>
#include "Client_test.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Client_test test;
    return a.exec();
}
