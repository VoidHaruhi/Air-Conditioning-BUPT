#include "db.h"
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
        database.setDatabaseName("test.db");
        database.setUserName("galaxy");
        database.setPassword("123456");
        //database = QSqlDatabase::;
    }
    query = QSqlQuery(database);

    if (!database.open())
    {
        qDebug() << "Failed to connect the database.";
    }

#if INITIAL
    initial();  // 仅需初始化一次
#endif
}

Database::~Database()
{
    database.close();
}

void Database::initial()
{
    // 房间固定信息表  主键—>roomId
    QString createSql = "CREATE TABLE room_info (roomId varchar(50) primary key, token varchar(50), defaultTmp int, createTime int);";
    if(!query.exec(createSql)){
        qDebug() << "Error in create table room_info." << query.lastError();
    }
    else{
        qDebug() << "Table room_info create !";
    }
    // 客户端基本信息表  主键->ip
    createSql = "CREATE TABLE ip_info (ip varchar(50) primary key, access varchar(50), id varchar(50));";
    if(!query.exec(createSql)){
        qDebug() << "Error in create table ip_info." << query.lastError();
    }
    else{
        qDebug() << "Table ip_info create !";
    }
    // 账户密码表  主键->username
    createSql = "CREATE TABLE usr_pwd (username varchar(50) primary key, pwd varchar(50), access varchar(50));";
    if(!query.exec(createSql)){
        qDebug() << "Error in create table usr_pwd." << query.lastError();
    }
    else{
        qDebug() << "Table usr_pwd create !";
    }
    // 房间状态表  主键->(roomId, beginTime)
    createSql = "CREATE TABLE room_status (roomId varchar(50), speed int, tmp int, nowTmp int, beginTime int, endTime int);";
    if(!query.exec(createSql)){
        qDebug() << "Error in create table room_status." << query.lastError();
    }
    else{
        qDebug() << "Table room_status create !";
    }
    // 详细计费表 主键->(roomId, time)
    createSql = "CREATE TABLE detailed_money (roomId varchar(50), token varchar(50), time int, money float);";
    if(!query.exec(createSql)){
        qDebug() << "Error in create table detailed_money." << query.lastError();
    }
    else{
        qDebug() << "Table detailed_money create !";
    }
    // 总费用表 主键->roomId
    createSql = "CREATE TABLE total_money (roomId varchar(50), token varchar(50), totalMoney float);";
    if(!query.exec(createSql)){
        qDebug() << "Error in create table total_money." << query.lastError();
    }
    else{
        qDebug() << "Table total_money create !";
    }

}

void Database::queryInsert(QString q)
{
    if(!query.exec(q)){
        qDebug() << "Error in INSERT for: " << q << query.lastError();
    }
    else{
        qDebug() << "INSERT succeed for " << q;
    }
}

void Database::queryDelete(QString q)
{
    if(!query.exec(q)){
        qDebug() << "Error in DELETE for: " << q << query.lastError();
    }
    else{
        qDebug() << "DELETE succeed for " << q;
    }
}

void Database::queryDrop(QString q)
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

void Database::queryDropAll()
{
    queryDrop("DROP TABLE room_info;");
    queryDrop("DROP TABLE ip_info;");
    queryDrop("DROP TABLE usr_pwd;");
    queryDrop("DROP TABLE room_status;");
    queryDrop("DROP TABLE detailed_money;");
    queryDrop("DROP TABLE total_money;");
}

QSqlQuery Database::querySelect(QString q, int mode)
{
    QSqlQuery query;
    query.exec(q);
    return query;
    /*
    QString temp = "null";
    switch (mode) {
    case 1: // 查询表1 room_info
    {
        break;
    }
    case 2: // 查询表2 ip_info
    {
        break;
    }
    case 3: // 查询表3 usr_pwd
    {
        // 该查询返回的结果是 数字+密码字符串 数字0表示经理，1表示管理员
        // 例如：结果为"0test001" 表示该用户为经理，密码是"test001"
        // 如果该用户不存在，返回temp初值"null"
        if(!query.exec(q)){
            qDebug() << "Error in SELECT for:" << q << query.lastError();
        }
        else{
            while(query.next()){
                if(query.value(2).toString() == "admin"){
                    temp += "1";
                }
                else
                    temp += "0";
                temp += query.value(1).toString();
            }
        }
        break;
    }
    case 4: // 查询表4 room_status
    {
        // 返回的数据具体格式后续定义
        break;
    }
    case 5: // 查询表5 detailed_money
    {
        // 返回的数据具体格式后续定义
        break;
    }
    case 6: // 查询表6 total_money
    {
        // 返回的数据具体格式后续定义
        break;
    }
    default:
        break;
    }

    qDebug() << temp;
    return temp;*/
}
