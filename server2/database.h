#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include "aircondition.h"

class Database
{
public:
    Database();
    ~Database();

    // 创建3个表格*/
    void dbInitial();
    // 查询语句，根据mode确定查询模式：对应6个表*/
    double dbInvoiceQuery(QString q);
    QList<RecordDetail> dbRDQuery(QString q);
    QList<QString> dbReportQuery();
    QList<QString> dbReportQuery1();
    // 插入语句，向表中插入数据*/
    void dbInsert(QString q);
    void dbUpdate(QString q);

    // 删除一个表，一般不用*/
    void dbDrop(QString q);
    // 清空数据库*/
    void dbDropAll();
    QSqlDatabase database;
    QSqlQuery query;
};

#endif // DATABASE_H
