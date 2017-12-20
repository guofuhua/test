#include <QCoreApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QVariantList>
#if 0

bool outPutTableInfo(QString tableNmae);
bool outputDatabaseInfo(QSqlDatabase databaseName);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("cashregistersystem.db");
    outputDatabaseInfo(database);

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

int test(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");

    database.setDatabaseName("CashSystem.db");

    if(database.open())
    {
        qDebug()<<"Database Opened";

        QSqlQuery sql_query;
        QString create_sql = "create table member (id int primary key, name varchar(30), address varchar(30))"; //创建数据表
        QString insert_sql = "insert into member values(?,?,?)";    //插入数据

        QString select_all_sql = "select * from member";

        sql_query.prepare(create_sql); //创建表
        if(!sql_query.exec()) //查看创建表是否成功
        {
            qDebug()<<QObject::tr("Table Create failed");
            qDebug()<<sql_query.lastError();
        }
        else
        {
            qDebug()<< "Table Created" ;

            //插入数据
            sql_query.prepare(insert_sql);

            QVariantList GroupIDs;
            GroupIDs.append(0);
            GroupIDs.append(1);
            GroupIDs.append(2);

            QVariantList GroupNames;
            GroupNames.append("hsp");
            GroupNames.append("rl");
            GroupNames.append("spl");

            QVariantList GroupAddress;
            GroupAddress.append("南充");
            GroupAddress.append("宝鸡");
            GroupAddress.append("南充");

            sql_query.addBindValue(GroupIDs);
            sql_query.addBindValue(GroupNames);
            sql_query.addBindValue(GroupAddress);

            if(!sql_query.execBatch())
            {
                qDebug()<<sql_query.lastError();
            }
            else
            {
                qDebug()<<"插入记录成功";
            }

            //查询所有记录
            sql_query.prepare(select_all_sql);
            if(!sql_query.exec())
            {
                qDebug()<<sql_query.lastError();
            }
            else
            {
                while(sql_query.next())
                {
                    int id = sql_query.value(0).toInt();
                    QString name = sql_query.value(1).toString();
                    QString address = sql_query.value(2).toString();
                    qDebug()<<QString("ID:%1  Name:%2  Address:%3").arg(id).arg(name).arg(address);
                }
            }
        }
    }
  /*  else
    {
        printf("open error---test\n");
    }*/
    database.close();
   // QFile::remove("CashSystem.db");
    return a.exec();
}
#endif
