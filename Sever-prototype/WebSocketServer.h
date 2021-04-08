#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include<QObject>
#include<QWidget>
#include<QWebSocketServer>
#include<QWebSocket>
#include<QString>
#include<QList>
#include<QJsonObject>
#include<map>
#include "airCondition.h"
#include "superUser.h"
#include "ui_WebSocketServer.h"
#include "db.h"

#define SEND_MESSAGE(sock,jsn) {QString _msg=QString(QJsonDocument(jsn).toJson());\
    if(sock->sendTextMessage(_msg)!=_msg.length())\
    {qDebug()<<"Send Error!";}}
#define WRONG_TOKEN "wrong token"
#define WRONG_ROOMID "wrong roonId"
#define GEN_ERROR(ret,tok) {ret.insert("handler","/server/error");ret.insert("msg",tok);}
using string=std::string;
namespace Ui {
class WebSocketServer;
}
class WebSocketServer:public QWidget{
    Q_OBJECT
public:
    explicit WebSocketServer(QWidget *parent = nullptr);
    ~WebSocketServer();
    /* 功能 */
    static QString generateToken();
    bool checkToken(QString token,QString role);
    QJsonObject roomInfoJson(AirCondition target);
    /* 接口 */
    void controlRoom(QWebSocket *socket, QJsonObject recvJson);
    void login(QWebSocket *socket, QJsonObject recvJson);
    void getRoomList(QWebSocket *socket, QJsonObject recvJson);
    void openRoom(QWebSocket *socket,QJsonObject recvJson);
    void seeRoomInfo(QWebSocket *socket,QJsonObject recvJson);
signals:
    void sendMessage(const QString &text);
    void process(QWebSocket *sock,QJsonObject recvJson);
private:
    void onNewConnection();
    void clearClient();
    void packageAnalyse(QWebSocket *socket, QJsonObject recvJson);

private:
    QWebSocketServer *server;
    QList<QWebSocket*> clientList;
    Ui::WebSocketServer *ui;
    Database w;

    std::map<int,QWebSocket*> sockMap;//roodId和房间websocket的字典，用于转发
    std::map<QString,QString> tokenMap;//token对role
    QList<AirCondition> airConditioners;//空调列表
    QList<SuperUser> superUsers;//管理员和经理的列表
};

#endif // WEBSOCKETSERVER_H
