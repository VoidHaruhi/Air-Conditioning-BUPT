#include "WebSocketServer.h"
#include "ui_WebSocketServer.h"
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

    int port;
    /*
    QFile file("D:\yuyue\University\SoftwareEngineering\Air-Conditioning-BUPT\build-prototype-Desktop_Qt_5_12_9_MSVC2017_64bit-Debug\debug\config.json");
    if ( !file.open( QIODevice::ReadWrite ) ) {
        qDebug() << "文件打开失败!\n";
        exit(1);
    }
    qDebug() << "文件打开成功\n";
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
    }*/
    port=12345;
    if(!server->listen(address,port)){
        qDebug()<<"Listen error";
    }else{
        qDebug()<<"Listening";
    }

    connect(server,&QWebSocketServer::newConnection,this,&WebSocketServer::onNewConnection);
    connect(this,&WebSocketServer::process,this,&WebSocketServer::packageAnalyse);
    SuperUser lyh;
    lyh.username="1";
    lyh.password="1";
    lyh.role="manager";
    superUsers.append(lyh);
//    AirCondition air;
//    air.idle=false;
//    air.wind=1;
//    air.setTmp=28;
//    air.defaultTmp=25;
//    air.roomId=304;
//    airConditioners.append(air);
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
    clientList.push_back(socket);//套接字加入客户端队列*/
    //收到消息*/
    connect(socket,&QWebSocket::textMessageReceived,[=](const QString &msg){
        ui->editRecv->append(msg);
        QString connectAddress = socket->peerAddress().toString();
        int connectPort = socket->peerPort();

            qDebug() <<"[New Package] From Address: " + connectAddress + "  Port: " + QString::number(connectPort) + " :";
            qDebug() << msg << endl;

            // 进行包的解析*/
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
    //发送消息*/
    //connect(this,&WebSocketServer::sendMessage,socket,&QWebSocket::sendTextMessage);
    //断开连接，释放*/
    connect(socket,&QWebSocket::disconnected,[this,socket](){
        clientList.removeAll(socket);
        socket->deleteLater();
    });
}
QString WebSocketServer::generateToken(){
    QString salt=QTime::currentTime().toString("zzz")+QString(QRandomGenerator::global()->bounded(100000));
    QString token=QCryptographicHash::hash(salt.toLatin1(),QCryptographicHash::Md5).toHex();
    qDebug()<<"Token generated"<<token<<salt;
    return token;
}
bool WebSocketServer::checkToken(QString token, QString role){
    if(tokenMap.count(token)==0){
        qDebug()<<"Wrong token "<<token << role;
        return false;
    }else if(tokenMap[token]!=role){
        qDebug()<<"Wrong role "<<token << role;
        return false;
    }
    return true;
}
int WebSocketServer::findRoom(int roomId){
    int len=airConditioners.length();
    for(int i=0;i<len;i++){
        if(airConditioners[i].roomId==roomId){
            return i;
        }
    }
    return -1;
}
void WebSocketServer::packageAnalyse(QWebSocket *socket, QJsonObject recvJson)
{
    QStringList request = recvJson["handler"].toString().split('/');
    QJsonObject answer;
    qDebug()<<request;
    if (request[1]=="manager"){//管理员发来消息*/
        if(request[2]=="controlRoom"){
            controlRoom(socket,recvJson);
        }else if(request[2]=="login"){
            login(socket,recvJson);
        }else if(request[2]=="getRoomList"){
            getRoomList(socket,recvJson);
        }else if(request[2]=="seeRoomInfo"){
            seeRoomInfo(socket,recvJson);
        }else if(request[2]=="openRoom"){
            openRoom(socket,recvJson);
        }else{
            qDebug()<<"request error";
        }
    }else if(request[1]=="client"){
        if(request[2]=="init"){
            initRoom(socket,recvJson);
        }else if(request[2]=="open"){
            openAc(socket,recvJson);
        }else if(request[2]=="set"){
            setAc(socket,recvJson);
        }else if(request[2]=="updataStatus"){
            updateAc(socket,recvJson);
        }else if(request[2]=="confirm"){

        }else{
            qDebug()<<"request error";
        }
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
    //回复管理员ACK*/
    ret.insert("refId",recvJson["refId"].toString());
    ret.insert("handler","/server/confirm");
    SEND_MESSAGE(socket,ret);
    //再向房间发送改变命令*/
    QJsonObject req;
    req.insert("refId",recvJson["refId"].toString());
    req.insert("handler","/server/setRoom");
    QJsonObject dataRet;
    dataRet.insert("token",token);
    dataRet.insert("defaultTmp",data["defaultTmp"].toInt());
    req.insert("data",dataRet);
    QWebSocket *socketR=sockMap[data["roomId"].toInt()];
    SEND_MESSAGE(socketR,req);
}
void WebSocketServer::login(QWebSocket *socket, QJsonObject recvJson){

    QJsonObject ret;
    ret.insert("refId",recvJson["refId"].toString());
    QJsonObject data=recvJson["data"].toObject();
    //检验*/
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
        //持久化*/


        dataRet.insert("token",token);
        ret.insert("data",dataRet);
        QString a;
    }
    SEND_MESSAGE(socket,ret);
}
void WebSocketServer::getRoomList(QWebSocket *socket, QJsonObject recvJson){
    QJsonObject ret;
    ret.insert("refId",recvJson["refId"].toString());
    QJsonObject data=recvJson["data"].toObject();
    QString token=recvJson["token"].toString();
    //检查token
    if(!checkToken(token,"manager")&&!checkToken(token,"admin")){
        GEN_ERROR(ret,WRONG_TOKEN);
        SEND_MESSAGE(socket,ret);
        return;
    }
    //返回*/
    ret.insert("handler","/server/retRoomList");
    QJsonArray roomList;
    //查询*/
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
    //为房间生成token*/
    QString token=generateToken();
    //存储*/
    QJsonObject data=recvJson["data"].toObject();
    //持久化*/
    int id=data["roomId"].toInt();
    int defaultTmp=data["defaultTmp"].toInt();
    int check=findRoom(id);
    if(check==-1){
        qDebug()<<"Wrong id";
        GEN_ERROR(ret,WRONG_ROOMID);
        SEND_MESSAGE(socket,ret);
        return;
    }
    airConditioners[check].open(token,defaultTmp);
    // ***** 将roomId和token加入映射表*/
    tokenMap[token]=data["roomId"].toString();
    // *****
    ret.insert("handler","/server/confirm");
    SEND_MESSAGE(socket,ret);
    QJsonObject req;
    req.insert("refId",recvJson["refId"].toString());
    req.insert("handler","/server/openRoom");
    QJsonObject dataReq;
    dataReq.insert("defaultTmp",defaultTmp);
    dataReq.insert("token",token);
    req.insert("data",dataReq);
    QWebSocket *socketR=sockMap[id];
    SEND_MESSAGE(socketR,req);

    // 更新房间固定信息表*/
    db.queryUpdate("UPDATE room_info set defaultTmp="+QString(defaultTmp)+
                  " ,token= '"+token+"' WHERE roomId="+QString(id));
//    QString q = "INSERT INTO room_info valus(";
//    q += QString(id)+token+QString(defaultTmp)+QString(ac->openTime)+");";
//    db.queryInsert(q);

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
    QJsonObject dataRet;
    QJsonObject data=recvJson["data"].toObject();
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
                //数据库查询*/
                AirCondition target=airConditioners[i];
                QJsonObject room=roomInfoJson(target);
                rooms.append(room);
                break;
            }
        }

    }
    dataRet.insert("roomInfoList",rooms);
    ret.insert("data",dataRet);
    SEND_MESSAGE(socket,ret);
}

void WebSocketServer::initRoom(QWebSocket *socket, QJsonObject recvJson)
{
    QJsonObject ret;
    ret.insert("refId",recvJson["refId"].toString());
    // 解析data*/
    QJsonObject data=recvJson["data"].toObject();
    int roomId=data["roomId"].toInt();
    int initTmp=data["initTmp"].toInt();
    // 查询该房间是否存在*/
    AirCondition *ac;
    int len=airConditioners.length();
    bool check=false;
    for(int i=0;i<len;i++){
        if(airConditioners[i].roomId==roomId){
            ac=&(airConditioners[i]);
            check=true;
            break;
        }
    }
    // 该房间不存在*/
    if(!check){
        AirCondition acTmp;
        acTmp.initial(roomId, initTmp);
        //test
        QString token=generateToken();
        acTmp.token=token;
        tokenMap[token]=QString("%1").arg(acTmp.roomId);
        // 将空调加入列表*/
        airConditioners.append(acTmp);
        qDebug()<<"Room:"<<roomId<<" is connected,"<<"initTmp is "<<initTmp;
        // 加入sockMap
        sockMap[roomId] = socket;
        // 回复消息*/
        ret.insert("handler","/server/confirm");
        SEND_MESSAGE(socket,ret);
        // 更新房间客户端信息表*/
//        QString q = "INSERT INTO ip_info values(";
//        QString ip = socket->peerAddress().toString();
//        q += "'"+ip +"'"+",'room',"+"'"+QString(roomId)+"'"+");";
//        db.queryInsert(q);

    }
    else{   // 回复错误信息*/
        qDebug()<<"Room:"<<roomId<<" is already exist.";
        ret.insert("handler","/server/error");
        ret.insert("msg","room already exists");
        SEND_MESSAGE(socket,ret);
    }
}

void WebSocketServer::setAc(QWebSocket *socket, QJsonObject recvJson)
{
    QJsonObject ret;
    ret.insert("refId",recvJson["refId"].toString());
    QString token = recvJson["token"].toString();
    // 判断该token是否存在*/
    int id;
    if(tokenMap.count(token)>0){
        id = tokenMap[token].toInt();
    }
    else{
        ret.insert("handler","/server/error");
        ret.insert("token",token);
        ret.insert("msg","token not exists");
        SEND_MESSAGE(socket,ret);
        return;
    }
    // 找到该房间对应的空调*/
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
    if(check){
        double tmp = recvJson["data"].toObject()["tmp"].toDouble();
        int wind = recvJson["data"].toObject()["wind"].toInt();
        ac->set(tmp, wind);
        ret.insert("handler","/server/confirm");
        ret.insert("token",token);
        SEND_MESSAGE(socket,ret);
    }

}

void WebSocketServer::openAc(QWebSocket *socket, QJsonObject recvJson)
{
    QJsonObject ret;
    ret.insert("refId",recvJson["refId"].toString());
    QString token = recvJson["token"].toString();
    // 判断该token是否存在*/
    int id;
    if(tokenMap.count(token)>0){
        id = tokenMap[token].toInt();
    }
    else{
        ret.insert("handler","/server/error");
        ret.insert("token",token);
        ret.insert("msg","token not exists");
        SEND_MESSAGE(socket,ret);
        return;
    }
    // 找到该房间对应的空调*/
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
    if(check){
        int power = recvJson["data"].toObject()["power"].toInt();
        if(power==0){
            ac->update();
            ac->close();
        }
        else{
            ac->start();
        }
        ret.insert("handler","/server/confirm");
        ret.insert("token",token);
        SEND_MESSAGE(socket,ret);
        // 更新数据库*/
        if(power==0){
//            QString q = "INSERT INTO detailed_money values (";
//            q += QString(ac->roomId)+","+token+","+QString(ac->lastUpdateTime)+","+QString("%1").arg(ac->totalFee) + ");";
//            db.queryInsert(q);
        }
    }
}

void WebSocketServer::updateAc(QWebSocket *socket, QJsonObject recvJson)
{
    QJsonObject ret;
    ret.insert("refId",recvJson["refId"].toString());
    QString token = recvJson["token"].toString();
    // 判断该token是否存在*/
    int id;
    if(tokenMap.count(token)>0){
        id = tokenMap[token].toInt();
    }
    else{
        ret.insert("handler","/server/error");
        ret.insert("token",token);
        ret.insert("msg","token not exists");
        SEND_MESSAGE(socket,ret);
        return;
    }
    // 找到该房间对应的空调*/
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
    if(check){
        double nowTmp = recvJson["data"].toObject()["nowTmp"].toDouble();
        int beginTime = ac->lastUpdateTime;
        ac->nowTmp = nowTmp;
        ac->update();
        // 构造报文*/
        ret.insert("handler","/server/confirm");
        ret.insert("token",token);
        QJsonObject data;
        data.insert("totalFee",ac->totalFee);
        ret.insert("data",data);
        SEND_MESSAGE(socket,ret);

        // 更新详细消费表*/
//        QString q = "INSERT INTO detailed_money values (";
//        q += QString(ac->roomId)+","+token+","+QString(ac->lastUpdateTime)+","+QString("%1").arg(ac->totalFee) + ");";
//        db.queryInsert(q);
//        // 更新房间状态表*/
//        q = "INSERT INTO room_status values (";
//        q += QString(ac->roomId)+","+QString(ac->wind)+","+QString("%1").arg(ac->nowTmp)+",";
//        q += QString(beginTime)+","+QString(ac->lastUpdateTime)+");";
//        db.queryInsert(q);
    }
}
