#include <QApplication>
#include "fileconvert.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileConvert w;
    w.show();
    
    return a.exec();
}
