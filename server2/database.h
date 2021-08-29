/**
 * @title   database.h
 * @module  声明持久化存储的类，持久化存储中央空调相关信息
 * @author  刘宇航
 * @interface   其他模块可以声明持久化存储对象，访问数据库读取和写入相关信息
 * @version  1.0    增加两个表
 *           1.1    增加报表的相关信息
 */

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

    /**
     * @brief dbInitial 初始化数据库，创建3个表
     */
    void dbInitial();
    /**
     * @brief dbInvoiceQuery 查询总消费
     * @param q sql查询语句
     * @return 消费金额
     */
    double dbInvoiceQuery(QString q);
    /**
     * @brief dbRDQuery 查询某一房间的详单
     * @param q sql查询语句
     * @return 详单列表
     */
    QList<RecordDetail> dbRDQuery(QString q);
    /**
     * @brief dbReportQuery 查询报表
     * @return 所有房间的报表信息
     */
    QList<QString> dbReportQuery();
    /**
     * @brief dbReportQuery1 查询账单
     * @return 所有房间的账单信息
     */
    QList<QString> dbReportQuery1();
    /**
     * @brief dbInsert 在某一个表中插入一项
     * @param q 插入一项的sql语句
     */
    void dbInsert(QString q);
    /**
     * @brief dbUpdate 更新一个表项中的相关内容
     * @param q 更新某一数据项的sql语句
     */
    void dbUpdate(QString q);
    /**
     * @brief dbDrop 删除一个表
     * @param q 删除一个表的sql语句
     */
    void dbDrop(QString q);
    /**
     * @brief dbDropAll 删除所有表项，情况数据库
     */
    void dbDropAll();
    /**
     * @brief database 数据库对象
     */
    QSqlDatabase database;
    /**
     * @brief query sql查询对象
     */
    QSqlQuery query;
};

#endif // DATABASE_H
