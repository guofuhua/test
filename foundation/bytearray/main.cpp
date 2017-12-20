#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QByteArray ori;
    qDebug() << ori.size();
    ori.resize(50);
    qDebug() << ori.size() << ori;
    ori.fill(0);
    qDebug() << ori.size() << ori;
    printf("%p\n", ori.constData());
    QByteArray new_array(1, '5');
    new_array.append((ori.mid(4,50)));
    qDebug() << new_array.size() << new_array;
    printf("%p\n", ori.constData());
    printf("%p\n", new_array.constData());
    return a.exec();
}
