#include <QCoreApplication>
#include <QStringList>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QVariantList>
bool outPutTableInfo(QString tableNmae);
bool outputDatabaseInfo(QSqlDatabase databaseName);
bool outPutTableTimeValue();

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    //database.setDatabaseName("RunRecord.db");
    database.setDatabaseName("mynew.db");
    if(database.open())
    {
        qDebug()<<"Database Opened";
        //QSqlQuery query(database);
        outputDatabaseInfo(database);
        outPutTableTimeValue();
        //query.QSqlQuery(database);
    }
    else
    {
        printf("open error---test\n");
    }
    database.close();
   // QFile::remove("CashSystem.db");
    return a.exec();
}

bool outputDatabaseInfo(QSqlDatabase database)
{
    if (database.open())
    {
        qDebug() << "Open database success!";
        QStringList tables = database.tables();  //获取数据库中的表
        qDebug() << QString::fromLocal8Bit("表的个数： %1").arg(tables.count()); //打印表的个数
        QStringListIterator itr(tables);
        while (itr.hasNext())
        {
            QString tableNmae = itr.next().toLocal8Bit();
            qDebug() << QString::fromLocal8Bit("表名：")+ tableNmae;

            outPutTableInfo(tableNmae);
        }
    }
    else
    {
        qDebug() << "Open database failed!";
        return false;
    }
    return true;
}

bool outPutTableInfo(QString tabNmae)
{
    QSqlQuery query;
    QString strTableNmae = tabNmae;
    QString str = "PRAGMA table_info(" + strTableNmae + ")";

    query.prepare(str);
    if (query.exec())
    {
        while (query.next())
        {
            qDebug() << QString(QString::fromLocal8Bit("字段数:%1     字段名:%2     字段类型:%3")).arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
        }
    }
    else
    {
        qDebug() << query.lastError();
        return false;
    }
    return true;
}

bool outPutTableTimeValue()
{
    QString str = "select * FROM RunRecord ORDER BY RecordDateTime ASC";
    QSqlQuery query;
    query.prepare(str);
    //qDebug() << "---------";
    if (query.exec())
    {//qDebug() << "---------";
        while (query.next())
        {//qDebug() << "-1-------";
            if (1 == query.value(26).toInt())
            {
            //qDebug() << QString("old_time:%1").arg(query.value(1).toString());
            qDebug() << QString("old_time:%1").arg(query.value(1).toString()) << QString("upload :flag:%1").arg(query.value(26).toString());
            }
        }
        //qDebug() << "-2------";
    }
    else
    {qDebug() << "---------";
        qDebug() << query.lastError();
        return false;
    }
    return true;
}
