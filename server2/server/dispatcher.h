#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <QList>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QDebug>
#include <iostream>
#include <map>
#include "header.h"
#include "aircondition.h"
#include "rcp_ctrl.h"
#include "mng_ctrl.h"
#include "scheduler.h"
#include "database.h"

#define GEN_ERROR(ret,tok) {ret.insert("handler",SERVER_ERROR);ret.insert("msg",tok);}


class Dispatcher:public QObject{
    Q_OBJECT

public:
    explicit Dispatcher(QObject *parent = nullptr);
    ~Dispatcher();
private:
    QWebSocketServer *server;
    QList<QWebSocket*> clientList;
    ReceptionController *receptionController;
    ManagerController *managerController;
    Scheduler *scheduler;
    Database *db;

    void onNewConnection();
    QJsonObject dispatch( QWebSocket *socket, QJsonObject msg);
    /*功能*/
    static QString generateToken();
    bool checkToken(QWebSocket *sock,QJsonObject recvJson, QString role);
    static QString generateMD5(QString str);
    static QString getToken(QJsonObject j);
    static QString getRoomId(QJsonObject j);
    /*roodId和房间websocket的字典，用于转发*/
    std::map<QString,QWebSocket*> sockMap;
    /*role:管理员为manager，经理为admin，房间为房间号*/
    std::map<QString,QString> tokenMap;


};

#endif // DISPATCHER_H
