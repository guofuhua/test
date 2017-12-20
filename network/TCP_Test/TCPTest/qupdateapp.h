#ifndef QUPDATEAPP_H
#define QUPDATEAPP_H

#include <QObject>

class QUpdateApp : public QObject
{
    Q_OBJECT
public:
    explicit QUpdateApp(QObject *parent = 0);
    bool checkUpgradeFile(QString appfile);
    int RunShellCommand(const char *command);
    bool md5Check();
    int RunUpDoFIleLine();

signals:
    
public slots:
    
};

#endif // QUPDATEAPP_H
