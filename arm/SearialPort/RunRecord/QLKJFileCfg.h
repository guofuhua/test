#ifndef QLKJFILECFG_H
#define QLKJFILECFG_H

#include <QSettings>
#include <QStringList>
#include <QMutex>

class QLKJFileCfg : public QObject
{
    Q_OBJECT
public:
    static QLKJFileCfg *getInstance() {
        static QLKJFileCfg instance;
        return &instance;
    }

    void setSyncFiles(QStringList names);
    int getFilesCount();
    QStringList getSyncFiles();
    void setUploadFile(QString file);
    QString getUploadFile();
    void setUploadNo(int No);
    int getUploadNo();

private:
    explicit QLKJFileCfg(QObject *parent = 0);
    QMutex m_mutex;

signals:

public slots:

};

#endif // QLKJFILECFG_H
