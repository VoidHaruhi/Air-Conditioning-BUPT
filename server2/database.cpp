#include "database.h"
#include <QDebug>

Database::Database()
{
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        database = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName("test6-7.db");
        database.setUserName("huohuo");
        database.setPassword("123456");
        //database = QSqlDatabase::;
    }
    query = QSqlQuery(database);

    if (!database.open())
    {
        qDebug() << "Failed to connect the database.";
    }
//    dbDropAll();
//    dbInitial();
}

Database::~Database()
{
    database.close();
}

void Database::dbInitial()
{
    //dbDropAll();

    QString createSql;
    /* 详细计费表 主键->roomId */
    createSql = "CREATE TABLE rd (roomId varchar(20), requestTime int, requestDuration int, \
                fanSpeed int, feeRate double, fee double);";
    if(!query.exec(createSql)){
        qDebug() << "Error in create table rd." << query.lastError();
    }
    else{
        qDebug() << "Table rd create !";
    }
    /* 总费用表 主键->roomId */
    createSql = "CREATE TABLE invoice (roomId varchar(20), totalFee double);";
    if(!query.exec(createSql)){
        qDebug() << "Error in create table invoice." << query.lastError();
    }
    else{
        qDebug() << "Table invoice create !";
    }
    /* 电费表 主键->roomId */
    createSql = "CREATE TABLE report (roomId varchar(20), switchTimes int, scheduleTimes int, \
                rdCount int, tempChangeTimes int, spdChangeTimes int, requestTime double, \
                totalFee double);";
    if(!query.exec(createSql)){
        qDebug() << "Error in create table report." << query.lastError();
    }
    else{
        qDebug() << "Table report create !";
    }

}

void Database::dbDrop(QString q)
{
    //dropSql = "DROP TABLE room_info1;";
    if(!query.exec(q))
    {
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "table cleared";
    }
}

void Database::dbDropAll()
{
    dbDrop("DROP TABLE rd;");
    dbDrop("DROP TABLE invoice;");
    dbDrop("DROP TABLE report;");
}

void Database::dbInsert(QString q)
{
    if(!query.exec(q)){
        qDebug() << "Error in INSERT for: " << q << query.lastError();
    }
    else{
        qDebug() << "INSERT succeed for " << q;
    }
}

void Database::dbUpdate(QString q)
{
    if(!query.exec(q)){
        qDebug() << "Error in Update for: " << q << query.lastError();
    }
    else{
        qDebug() << "Update succeed for " << q;
    }
}

QList<RecordDetail> Database::dbRDQuery(QString q)
{
    QList<RecordDetail> rd;
    RecordDetail item;
    if(!query.exec(q)){
            qDebug() << "Error in Select for: " << q << query.lastError();
    }
    else{
        while(query.next()){
            item.roomId = query.value(0).toString();
            item.requestTime = query.value(1).toInt();
            item.requestDuration = query.value(2).toInt();
            item.fanSpeed = query.value(3).toInt();
            item.feeRate = query.value(4).toDouble();
            item.fee = query.value(5).toDouble();
            rd.append(item);
        }
    }
    return rd;
}

double Database::dbInvoiceQuery(QString q)
{
    double fee=-1;
    if(!query.exec(q)){
            qDebug() << "Error in Select for: " << q << query.lastError();
    }
    else{
        while(query.next()){
            fee = query.value(1).toDouble();
        }
    }
    return fee;
}

QList<QString> Database::dbReportQuery()
{
    QList<QString> roomList;
    QString q = "Select * from report";
    if(!query.exec(q)){
            qDebug() << "Error in Select for: " << q << query.lastError();
    }
    else{
        while(query.next()){
            QString temp = "";
            temp += query.value(0).toString();
            temp += '/';
            int tempInt = query.value(1).toInt();
            temp += QString("%1").arg(tempInt);
            temp += '/';
            tempInt = query.value(2).toInt();
            temp += QString("%1").arg(tempInt);
            temp += '/';
            tempInt = query.value(3).toInt();
            temp += QString("%1").arg(tempInt);
            temp += '/';
            tempInt = query.value(4).toInt();
            temp += QString("%1").arg(tempInt);
            temp += '/';
            tempInt = query.value(5).toInt();
            temp += QString("%1").arg(tempInt);
            temp += '/';
            double tempDouble = query.value(6).toDouble();
            temp += QString("%1").arg(tempDouble);
            temp += '/';
            tempDouble = query.value(7).toInt();
            temp += QString("%1").arg(tempDouble);

            roomList.append(temp);
        }
    }
    return roomList;
}

QList<QString> Database::dbReportQuery1()
{
    QList<QString> roomList;
    QString q = "Select * from invoice";
    if(!query.exec(q)){
            qDebug() << "Error in Select for: " << q << query.lastError();
    }
    else{
        while(query.next()){
            QString temp = "";
            temp += query.value(0).toString();
            temp += '/';
            double fee = query.value(1).toDouble();
            temp += QString("%1").arg(fee);
            roomList.append(temp);
        }
    }
    return roomList;
}
