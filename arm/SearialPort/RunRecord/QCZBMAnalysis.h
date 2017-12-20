#ifndef QCZBMANALYSIS_H
#define QCZBMANALYSIS_H

#include <QObject>
//******************************************************************************
//类名:QCZBMAnalysis
//功能:站名表解析
//备注:
//******************************************************************************
class QCZBMAnalysis : public QObject
{
    Q_OBJECT
public:
    explicit QCZBMAnalysis(QObject *parent = 0);
    ~QCZBMAnalysis();
    //得到站名
    QString GetStationName(uchar ByteJiaoLu/*交路*/, int siStationID/*车站号*/);
private:
    //TXMLFile m_XMLDoc;
    QString m_strFileName;
signals:
    
public slots:
    
};

#endif // QCZBMANALYSIS_H
