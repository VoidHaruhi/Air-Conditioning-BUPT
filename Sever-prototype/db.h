#ifndef DB_H
#define DB_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>

#define INITIAL 1
class Database
{

public:
    Database();
    ~Database();

    // 创建6个
    void initial();
    // 查询语句，根据mode确定查询模式：对应6个表
    QSqlQuery querySelect(QString q, int mode);
    // 插入语句，向表中插入数据
    void queryInsert(QString q);
    // 删除语句，删除所选项
    void queryDelete(QString q);
    // 删除一个表，一般不用
    void queryDrop(QString q);
    // 清空数据库
    void queryDropAll();
    QSqlDatabase database;
    QSqlQuery query;
};


#endif // DB_H
