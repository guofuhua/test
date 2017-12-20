#include <QCoreApplication>
#include <QTest1.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTest1 *test = new QTest1();
    test->Test();
    sleep(1);
    test->TestEnd();
    
    sleep(20);
    delete test;

    return a.exec();
}
