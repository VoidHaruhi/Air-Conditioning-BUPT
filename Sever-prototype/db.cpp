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
    queryDropAll();
    initial();  // 仅需初始化一次*/
#endif
}

Database::~Database()
{
    database.close();
}

void Database::initial()
{
    // 房间固定信息表  主键—>roomId*/
    QString createSql = "CREATE TABLE room_info "
                        "(roomId varchar(50) primary key, token varchar(50), defaultTmp double, initTmp double, createTime int);";
    if(!query.exec(createSql)){
        qDebug() << "Error in create table room_info." << query.lastError();
    }
    else{
        qDebug() << "Table room_info create !";
    }
    // 客户端基本信息表  主键->ip*/
    createSql = "CREATE TABLE ip_info (ip varchar(50) primary key, access varchar(50), id varchar(50));";
    if(!query.exec(createSql)){
        qDebug() << "Error in create table ip_info." << query.lastError();
    }
    else{
        qDebug() << "Table ip_info create !";
    }
    // 账户密码表  主键->username*/
    createSql = "CREATE TABLE usr_pwd (username varchar(50) primary key, pwd varchar(50), access varchar(50));";
    if(!query.exec(createSql)){
        qDebug() << "Error in create table usr_pwd." << query.lastError();
    }
    else{
        qDebug() << "Table usr_pwd create !";
    }
    // 房间状态表  主键->(roomId, beginTime)*/
    createSql = "CREATE TABLE room_status (roomId varchar(50), speed int, tmp double, nowTmp double, time int,money double);";
    if(!query.exec(createSql)){
        qDebug() << "Error in create table room_status." << query.lastError();
    }
    else{
        qDebug() << "Table room_status create !";
    }
    // 详细计费表 主键->(roomId, time)*/
    createSql = "CREATE TABLE detailed_money (roomId varchar(50), token varchar(50), time int, money double);";
    if(!query.exec(createSql)){
        qDebug() << "Error in create table detailed_money." << query.lastError();
    }
    else{
        qDebug() << "Table detailed_money create !";
    }
    // 总费用表 主键->roomId*/
    createSql = "CREATE TABLE total_money (roomId varchar(50), token varchar(50), totalMoney double);";
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

QSqlQuery Database::querySelect(QString q)
{
    QSqlQuery query;
    query.exec(q);
    return query;
}
void Database::queryUpdate(QString q){
    if(!query.exec(q)){
        qDebug() << "Error in UPDATE for: " << q << query.lastError();
    }
    else{
        qDebug() << "UPDATE succeed for " << q;
    }
}
