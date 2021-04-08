#include "WebSocketServer.h"

#include <QFile>
#include <QString>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QCryptographicHash>
#include <QRandomGenerator64>
#include <time.h>

WebSocketServer::WebSocketServer(QWidget *parent):
    QWidget(parent),
    ui(new Ui::WebSocketServer)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    ui->setupUi(this);
    server=new QWebSocketServer("Server",QWebSocketServer::NonSecureMode,this);
    QHostAddress address=QHostAddress::Any;
    QFile file("config.json");
    if ( !file.open( QIODevice::ReadWrite ) ) {
        qDebug() << "文件打开失败!\n";
        exit(1);
    }
    qDebug() << "文件打开成功\n";
    int port;
    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseJsonErr);
    if(!(parseJsonErr.error==QJsonParseError::NoError)){
        qDebug()<<"Port parse error";
        exit(1);
    }else{
        QJsonObject j=document.object();
        if(j.contains("port")){
            port=j["port"].toInt();
        }
    }
    if(server->listen(address,port)){
        qDebug()<<"Listen error";
        exit(1);
    }
    connect(server,&QWebSocketServer::newConnection,this,&WebSocketServer::onNewConnection);
    connect(this,&WebSocketServer::process,this,&WebSocketServer::packageAnalyse);

}
WebSocketServer::~WebSocketServer(){
    clearClient();
    server->close();
    delete ui;
}
void WebSocketServer::clearClient()
{
    for(int i=clientList.count()-1;i>=0;i--)
    {
        //qDebug()<<i;
        clientList.at(i)->disconnect();
        clientList.at(i)->close();
    }
    qDeleteAll(clientList);
    clientList.clear();
}
void WebSocketServer::onNewConnection(){
    QWebSocket *socket=server->nextPendingConnection();
    if(!socket)
        return;
    clientList.push_back(socket);//套接字加入客户端队列
    //收到消息
    connect(socket,&QWebSocket::textMessageReceived,[=](QString &msg){
        ui->editRecv->append(msg);
        QString connectAddress = socket->peerAddress().toString();
        int connectPort = socket->peerPort();

            qDebug() <<"[New Package] From Address: " + connectAddress + "  Port: " + QString::number(connectPort) + " :";
            qDebug() << msg << endl;

            // 进行包的解析
            QByteArray recvPackage = msg.toUtf8();
            QJsonParseError parseJsonErr;
            QJsonDocument document = QJsonDocument::fromJson(recvPackage, &parseJsonErr);
            if(!(parseJsonErr.error == QJsonParseError::NoError))
            {
                qDebug()<<"解析json文件错误.";
                return;
            }
            packageAnalyse(socket,document.object());
            //emit process(socket,document.object());

    });
    //发送消息
    //connect(this,&WebSocketServer::sendMessage,socket,&QWebSocket::sendTextMessage);
    //断开连接，释放
    connect(socket,&QWebSocket::disconnected,[this,socket](){
        clientList.removeAll(socket);
        socket->deleteLater();
    });
}
QString WebSocketServer::generateToken(){
    QString salt=QString(int(clock()))+QString(qrand());
    return QCryptographicHash::hash(salt.toLatin1(),QCryptographicHash::Md5).toHex();
}
bool WebSocketServer::checkToken(QString token, QString role){
    if(tokenMap.count(token)==0||tokenMap[token]!=role){
        return false;
    }
    return true;
}
void WebSocketServer::packageAnalyse(QWebSocket *socket, QJsonObject recvJson)
{
    QStringList request = recvJson["handler"].toString().split('/');
    QJsonObject answer;
    if (request[0]=="manager"){//管理员发来消息
        if(request[1]=="controlRoom"){
            controlRoom(socket,recvJson);
        }else if(request[1]=="login"){
            login(socket,recvJson);
        }
    }else if(request[0]=="client"){

    }else{
        qDebug()<<"Wrong handler!";
        return;
    }
}
void WebSocketServer::controlRoom(QWebSocket *socket, QJsonObject recvJson){
    QJsonObject ret;
    QString token=recvJson["token"].toString();
    if(!checkToken(token,"manager")){
        qDebug()<<"Wrong token";
        GEN_ERROR(ret,WRONG_TOKEN);
        SEND_MESSAGE(socket,ret);
        return;
    }
    if(!recvJson.contains("data")){
        qDebug()<<"Error in controlRoom: No data field!";
        return;
    }
    QJsonObject data=recvJson["data"].toObject();
    //回复管理员ACK
    ret.insert("refId",recvJson["refId"].toInt());
    ret.insert("handler","/server/comfirm");
    SEND_MESSAGE(socket,ret);
    //再向房间发送改变命令
    QJsonObject req;
    req.insert("refId",recvJson["refId"].toInt());
    req.insert("handler","/server/setRoom");
    req.insert("token",token);
    req.insert("data",recvJson["data"].toObject());
    QWebSocket *socketR=sockMap[data["roomId"].toInt()];
    SEND_MESSAGE(socketR,req);

}
void WebSocketServer::login(QWebSocket *socket, QJsonObject recvJson){

    QJsonObject ret;
    ret.insert("refId",recvJson["refId"].toInt());
    QJsonObject data=recvJson["data"].toObject();
    //检验
    bool check=false;
    int len=superUsers.length();
    SuperUser *target;
    for(int i=0;i<len;i++){
        if(superUsers[i].password==data["password"].toString()){
            target=&(superUsers[i]);
            check=true;
            break;
        }
    }
    if(!check){
       ret.insert("handler","/server/error");
       ret.insert("msg","wrong password");
    }else{
        QString role=target->role;
        ret.insert("handler","/server/retRole");
        QJsonObject dataRet;
        dataRet.insert("role",role);
        QString token=generateToken();
        //存token
        target->token=token;
        tokenMap[token]=role;
        //持久化


        dataRet.insert("token",token);
        ret.insert("data",dataRet);
        QString a;
    }
    SEND_MESSAGE(socket,ret);
}
void WebSocketServer::getRoomList(QWebSocket *socket, QJsonObject recvJson){
    QJsonObject ret;
    ret.insert("refId",recvJson["refId"].toInt());
    QJsonObject data=recvJson["data"].toObject();
    QString token=recvJson["token"].toString();
    //检查token
    if(!checkToken(token,"manager")&&!checkToken(token,"admin")){
        GEN_ERROR(ret,WRONG_TOKEN);
        SEND_MESSAGE(socket,ret);
        return;
    }
    //返回
    ret.insert("handler","/server/retRoomList");
    QJsonArray roomList;
    //查询
    int len=airConditioners.length();
    for(int i=0;i<len;i++){
        QJsonObject room;
        room.insert("roomId",airConditioners[i].roomId);
        room.insert("idle",airConditioners[i].idle);
        roomList.append(room);
    }
    QJsonObject dataRet;
    dataRet.insert("roomList",roomList);
    ret.insert("data",dataRet);

    SEND_MESSAGE(socket,ret);
}
void WebSocketServer::openRoom(QWebSocket *socket, QJsonObject recvJson){
    QJsonObject ret;
    ret.insert("refId",recvJson["refId"].toString());
    if(!checkToken(recvJson["token"].toString(),"manager")){
        GEN_ERROR(ret,WRONG_TOKEN);
        SEND_MESSAGE(socket,ret);
        return;
    }
    //为房间生成token
    QString token=generateToken();
    //存储
    QJsonObject data=recvJson["data"].toObject();
    //持久化

    int id=data["roomId"].toInt();
    int defaultTmp=data["defaultTmp"].toInt();
    AirCondition *ac;
    int len=airConditioners.length();
    bool check=false;
    for(int i=0;i<len;i++){
        if(airConditioners[i].roomId==id){
            ac=&(airConditioners[i]);
            check=true;
            break;
        }
    }
    if(!check){
        qDebug()<<"Wrong id";
        GEN_ERROR(ret,WRONG_ROOMID);
        SEND_MESSAGE(socket,ret);
        return;
    }
    ret.insert("handler","/server/comfirm");
    SEND_MESSAGE(socket,ret);
    QJsonObject req;
    req.insert("refId",recvJson["refId"].toInt());
    req.insert("handler","/server/openRoom");
    QJsonObject dataReq;
    dataReq.insert("defaultTmp",defaultTmp);
    dataReq.insert("token",token);
    req.insert("data",dataReq);
    QWebSocket *socketR=sockMap[id];
    SEND_MESSAGE(socketR,req);
    //等待回复？
}
QJsonObject WebSocketServer::roomInfoJson(AirCondition target){
    QJsonObject room;
    room.insert("roomId",target.roomId);
    room.insert("power",int(target.power));
    room.insert("setTmp",int(target.setTmp));
    room.insert("nowTmp",int(target.nowTmp));
    room.insert("wind",target.wind);
    return room;
}
void WebSocketServer::seeRoomInfo(QWebSocket *socket, QJsonObject recvJson){
    QJsonObject ret;
    ret.insert("refId",recvJson["refId"].toString());
    if(!checkToken(recvJson["token"].toString(),"manager")){
        GEN_ERROR(ret,WRONG_TOKEN);
        SEND_MESSAGE(socket,ret);
        return;
    }
    ret.insert("handler","/server/roomInfo");
    QJsonArray dataRet;
    QJsonObject data=ret["data"].toObject();
    QJsonArray rooms;
    int len=airConditioners.length();
    if(data["roomId"].toString()=="All"){
        for(int i=0;i<len;i++){
            if(airConditioners[i].idle==false){
                QJsonObject room=roomInfoJson(airConditioners[i]);
                rooms.append(room);
            }
        }
    }else{
        int id=data["roomId"].toInt();
        for(int i=0;i<len;i++){
            if(airConditioners[i].roomId==id){
                //数据库查询
                AirCondition target=airConditioners[i];
                QJsonObject room=roomInfoJson(target);
                rooms.append(room);
                break;
            }
        }

    }
    ret.insert("roomInfoList",rooms);
    SEND_MESSAGE(socket,ret);
}




