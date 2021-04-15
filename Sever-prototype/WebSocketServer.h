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
#include "db.h"
#include "headers.h"

#define SEND_MESSAGE(sock,jsn) {QString _msg=QString(QJsonDocument(jsn).toJson());\
    if(sock->sendTextMessage(_msg)!=_msg.length())\
    {qDebug()<<"Send Error!";}\
    qDebug()<<_msg;}
#define GEN_ERROR(ret,tok) {ret.insert("handler",SERVER_ERROR);ret.insert("msg",tok);}
namespace Ui {
class WebSocketServer;
}
class WebSocketServer:public QWidget{
    Q_OBJECT
public:
    explicit WebSocketServer(QWidget *parent = nullptr);
    ~WebSocketServer();
    //功能*/
    static QString generateToken();
    bool checkToken(QWebSocket *sock,QJsonObject recvJson, QString role);
    static QString generateMD5(QString str);
    QJsonObject roomInfoJson(AirCondition target);
    int findRoom(QString roomId);//找房间
    QString getRefId(QJsonObject j);
    static QString getRoomId(QJsonObject j);
    static QString getToken(QJsonObject j);
    static int getCurrentTime();//单位为秒
    double getCostOfRoom(QString id);
    QJsonArray getCostListOfRoom(QString id);
    void loadSuperUsers();

    //定时
    void updateCostInfo();

    //接口*/
    void controlRoom(QWebSocket *socket, QJsonObject recvJson);
    void login(QWebSocket *socket, QJsonObject recvJson);
    void logout(QWebSocket *socket, QJsonObject recvJson);
    void getRoomList(QWebSocket *socket, QJsonObject recvJson);
    void openRoom(QWebSocket *socket,QJsonObject recvJson);
    void seeRoomInfo(QWebSocket *socket,QJsonObject recvJson);
    void simpleCost(QWebSocket *socket,QJsonObject recvJson);
    void detailCost(QWebSocket *socket,QJsonObject recvJson);
    void requestReport(QWebSocket *socket,QJsonObject recvJson);

    void initRoom(QWebSocket *socket,QJsonObject recvJson);
    void controlAc(QWebSocket *socket,QJsonObject recvJson);
    void updateAc(QWebSocket *socket,QJsonObject recvJson);
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
    Database db;

    /*roodId和房间websocket的字典，用于转发*/
    std::map<QString,QWebSocket*> sockMap;
    /*role:管理员为manager，经理为admin，房间为房间号*/
    std::map<QString,QString> tokenMap;
    QList<AirCondition> airConditioners;/*空调列表*/
    QList<SuperUser> superUsers;/*管理员和经理的列表*/
};

#endif // WEBSOCKETSERVER_H
