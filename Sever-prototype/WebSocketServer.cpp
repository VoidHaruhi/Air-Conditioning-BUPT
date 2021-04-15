#include "WebSocketServer.h"
#include "ui_WebSocketServer.h"
#include "arguments.h"
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
#include <QTime>
#include <QTimer>
#include <QTimerEvent>

WebSocketServer::WebSocketServer(QWidget *parent):
    QWidget(parent),
    ui(new Ui::WebSocketServer)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    ui->setupUi(this);
    server=new QWebSocketServer("Server",QWebSocketServer::NonSecureMode,this);
    QHostAddress address=QHostAddress::Any;

    int port;
    port=12345;
    if(!server->listen(address,port)){
        qDebug()<<"Listen error";
    }else{
        qDebug()<<"Listening";
    }
    //timer
    QTimer *timer=new QTimer(this);
    timer->setInterval(60*1000);
    timer->start();
    connect(timer,&QTimer::timeout,this,&WebSocketServer::updateCostInfo);
    connect(server,&QWebSocketServer::newConnection,this,&WebSocketServer::onNewConnection);
    connect(this,&WebSocketServer::process,this,&WebSocketServer::packageAnalyse);
    loadSuperUsers();
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
            //packageAnalyse(socket,document.object());
            emit process(socket,document.object());

    });
    //断开连接，释放*/
    connect(socket,&QWebSocket::disconnected,[this,socket](){
        clientList.removeAll(socket);
        socket->deleteLater();
    });
}
QString WebSocketServer::generateToken(){
    QString salt=QTime::currentTime().toString("zzz")+QString("%1").arg(QRandomGenerator::global()->bounded(10000000));
    QString token=QCryptographicHash::hash(salt.toLatin1(),QCryptographicHash::Md5).toHex();
    qDebug()<<"Token generated"<<token<<salt;
    return token;
}
QString WebSocketServer::generateMD5(QString str){
    return QCryptographicHash::hash(str.toLatin1(),QCryptographicHash::Md5).toHex();
}
bool WebSocketServer::checkToken(QWebSocket *sock,QJsonObject recvJson, QString role){
    QString token=getToken(recvJson);
    bool flag=true;
    QJsonObject ret;
    if(tokenMap.count(token)==0){
        qDebug()<<"Wrong token "<<token << role;
        GEN_ERROR(ret,WRONG_TOKEN);
        flag=false;
    }else if(tokenMap[token]!=role){
        qDebug()<<"Wrong role "<<token << role;
        GEN_ERROR(ret,WRONG_ROLE);
        flag=false;
    }
    if(!flag)
        SEND_MESSAGE(sock,ret);
    return flag;
}
int WebSocketServer::findRoom(QString roomId){
    int len=airConditioners.length();
    for(int i=0;i<len;i++){
        if(airConditioners[i].roomId==roomId){
            return i;
        }
    }
    return -1;
}
QString WebSocketServer::getRefId(QJsonObject j){
    if(!j.contains(REFID)){
        qDebug()<<"NO field refId";
        return NULLSTRING;
    }
    return j[REFID].toString();
}

QString WebSocketServer::getToken(QJsonObject j){
    if(!j.contains("token")){
        qDebug()<<"No field token";
        return "null";
    }
    return j["token"].toString();
}

QString WebSocketServer::getRoomId(QJsonObject j){
    if(!j.contains("data")){
        qDebug()<<"No data field";
        return NULLSTRING;
    }
    QJsonObject jd=j["data"].toObject();
    if(!jd.contains(ROOM_ID)){
        qDebug()<<"No field roomId";
        return NULLSTRING;
    }
    return jd[ROOM_ID].toString();
}

double WebSocketServer::getCostOfRoom(QString id){
    int idx=findRoom(id);
    if(idx==-1){
        qDebug()<<"No room found";
        return -1;
    }
    AirCondition *ac=&(airConditioners[idx]);
    return ac->totalFee+ac->getNewFee();
}

QJsonArray WebSocketServer::getCostListOfRoom(QString id){
    QJsonArray ret;
    int idx=findRoom(id);
    if(idx==-1){
        qDebug()<<"No room found";
        return ret;
    }
    QSqlQuery res=db.querySelect(QString("SELECT time,nowTmp,setTmp,wind,money FROM room_status WHERE roomId='%1'"
                                 "ORDERED BY time ASEC").arg(id));
    while(!res.next()){
        QJsonObject row;
        row.insert("time",res.value(0).toInt());
        row.insert("nowTmp",res.value(1).toDouble());
        row.insert("setTmp",res.value(2).toDouble());
        row.insert("wind",res.value(3).toInt());
        row.insert("money",res.value(4).toDouble());
        ret.append(row);
    }
    return ret;
}

void WebSocketServer::updateCostInfo(){
    static int cnt=0;
    int i;
    for(i=0;i<airConditioners.length();i++){
        AirCondition *ac=&(airConditioners[i]);
        double fee=ac->totalFee+ac->getNewFee()-ac->lastFee;
        ac->lastFee=ac->totalFee+ac->getNewFee();
        db.queryInsert(QString("INSERT INTO room_status(roomId,time,nowTmp,setTmp,wind,money) "
                       "VALUES ('%1',%2,%3,%4,%5,%6)")
                       .arg(ac->roomId).arg(cnt).arg(ac->nowTmp).arg(ac->setTmp).arg(fee).arg(ac->wind));
    }
    cnt++;
    qDebug()<<"Costs of rooms are updated";
}

int WebSocketServer::getCurrentTime(){
    QTime time=QTime::currentTime();
    return time.hour()*60+time.minute();
}

void WebSocketServer::loadSuperUsers(){
    QFile file(":superuser/users.txt");
          if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
          {
              qDebug()<<"Open file error!";
              return;
            }
          while (!file.atEnd()) {
//              QByteArray line = file.readLine();
              QString line = file.readLine();
              QStringList list = line.split(" ");
              SuperUser newuser;
              newuser.username=list[0];
              newuser.password=generateMD5(list[1]);
              newuser.role=list[2];
              superUsers.append(newuser);
    }
}
void WebSocketServer::packageAnalyse(QWebSocket *socket, QJsonObject recvJson)
{
    QStringList request = recvJson[HANDLER].toString().split('/');
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
        }else if(request[2]=="simpleCost"){
            simpleCost(socket,recvJson);
        }else if(request[2]=="detailCost"){
            detailCost(socket,recvJson);
        }else{
            qDebug()<<"request error";
        }
    }else if(request[1]=="client"){
        if(request[2]=="init"){
            initRoom(socket,recvJson);
        }else if(request[2]=="controlRoom"){
            controlAc(socket,recvJson);
        }else if(request[2]=="updateStatus"){
            updateAc(socket,recvJson);
        }else if(request[2]==CONFIRM){

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
    QString token=recvJson[TOKEN].toString();
    if(!checkToken(socket,recvJson,"manager")){
        return;
    }
    if(!recvJson.contains(DATA)){
        qDebug()<<"Error in controlRoom: No data field!";
        return;
    }
    QJsonObject data=recvJson[DATA].toObject();
    //回复管理员ACK*/
    ret.insert(REFID,getRefId(recvJson));
    ret.insert(HANDLER,SERVER_CONFIRM);
    SEND_MESSAGE(socket,ret);
    //再向房间发送改变命令*/
    QJsonObject req;
    req.insert(REFID,getRefId(recvJson));
    req.insert(HANDLER,"/server/setRoom");
    req.insert(DATA,recvJson["data"]);
    QWebSocket *socketR=sockMap[getRoomId(recvJson)];
    SEND_MESSAGE(socketR,req);
}
void WebSocketServer::login(QWebSocket *socket, QJsonObject recvJson){

    QJsonObject ret;
    ret.insert(REFID,getRefId(recvJson));
    QJsonObject data=recvJson[DATA].toObject();
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
        ret.insert(HANDLER,SERVER_ERROR);
        ret.insert("msg","wrong password");
    }else{
        QString role=target->role;
        ret.insert(HANDLER,"/server/retRole");
        QJsonObject dataRet;
        dataRet.insert("role",role);
        QString token=generateToken();
        //存token
        target->token=token;
        tokenMap[token]=role;
        //持久化*/


        dataRet.insert(TOKEN,token);
        ret.insert(DATA,dataRet);
        QString a;
    }
    SEND_MESSAGE(socket,ret);
}
void WebSocketServer::logout(QWebSocket *socket, QJsonObject recvJson){
    //检查token
    if(!checkToken(socket,recvJson,MANAGER))
        return;
    QString token=getToken(recvJson);
    QJsonObject ret;
    ret.insert(HANDLER,SERVER_CONFIRM);
    ret.insert(REFID,getRefId(recvJson));
    SEND_MESSAGE(socket,ret);
    //删除对应token
    tokenMap.erase(token);
}
void WebSocketServer::getRoomList(QWebSocket *socket, QJsonObject recvJson){
    QJsonObject ret;
    ret.insert(REFID,getRefId(recvJson));
    QJsonObject data=recvJson[DATA].toObject();
    //检查token
    if(!checkToken(socket,recvJson,MANAGER))
        return;
    //返回*/
    ret.insert(HANDLER,"/server/retRoomList");
    QJsonArray roomList;
    //查询*/
    int len=airConditioners.length();
    for(int i=0;i<len;i++){
        QJsonObject room;
        room.insert(ROOM_ID,airConditioners[i].roomId);
        room.insert("idle",airConditioners[i].idle);
        roomList.append(room);
    }
    QJsonObject dataRet;
    dataRet.insert("roomList",roomList);
    ret.insert(DATA,dataRet);
    SEND_MESSAGE(socket,ret);
}
void WebSocketServer::openRoom(QWebSocket *socket, QJsonObject recvJson){
    QJsonObject ret;
    ret.insert(REFID,getRefId(recvJson));
    //检查token
    if(!checkToken(socket,recvJson,MANAGER))
        return;
    //为房间生成token*/
    QString token=generateToken();
    //存储*/
    QJsonObject data=recvJson[DATA].toObject();
    //持久化*/
    QString id=getRoomId(recvJson);
    double defaultTmp=data[DEFAULT_TMP].toDouble();
    int check=findRoom(id);
    if(check==-1){
        qDebug()<<"Wrong id";
        GEN_ERROR(ret,WRONG_ROOMID);
        SEND_MESSAGE(socket,ret);
        return;
    }
    airConditioners[check].open(token,defaultTmp);
    // ***** 将roomId和token加入映射表*/
    tokenMap[token]="room";
    // *****
    ret.insert(HANDLER,SERVER_CONFIRM);
    SEND_MESSAGE(socket,ret);
    QJsonObject req;
    req.insert(REFID,getRefId(recvJson));
    req.insert(HANDLER,"/server/openRoom");
    QJsonObject dataReq;
    dataReq.insert(DEFAULT_TMP,defaultTmp);
    dataReq.insert(TOKEN,token);
    req.insert(DATA,dataReq);
    QWebSocket *socketR=sockMap[id];
    SEND_MESSAGE(socketR,req);

    // 更新房间固定信息表*/
    db.queryUpdate("UPDATE room_info set defaultTmp="+QString("%1").arg(defaultTmp)+
                  " ,token= '"+token+"' WHERE roomId='"+id+"'");
}
QJsonObject WebSocketServer::roomInfoJson(AirCondition target){
    QJsonObject room;
    room.insert(ROOM_ID,target.roomId);
    room.insert("power",int(target.power));
    room.insert("setTmp",double(target.setTmp));
    room.insert("nowTmp",double(target.nowTmp));
    room.insert("wind",int(target.wind));
    room.insert("fee",double(target.totalFee+target.getNewFee()));
    return room;
}
void WebSocketServer::seeRoomInfo(QWebSocket *socket, QJsonObject recvJson){
    QJsonObject ret;
    ret.insert(REFID,getRefId(recvJson));
    //检查token
    if(!checkToken(socket,recvJson,MANAGER))
        return;
    ret.insert(HANDLER,"/server/roomInfo");
    QJsonObject dataRet;
    QJsonObject data=recvJson[DATA].toObject();
    QJsonArray rooms;
    int len=airConditioners.length();
    if(data[ROOM_ID].toString()=="All"){
        for(int i=0;i<len;i++){
            if(airConditioners[i].idle==false){
                QJsonObject room=roomInfoJson(airConditioners[i]);
                rooms.append(room);
            }
        }
    }else{
        QString id=getRoomId(recvJson);
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
    ret.insert(DATA,dataRet);
    SEND_MESSAGE(socket,ret);
}
void WebSocketServer::simpleCost(QWebSocket *socket, QJsonObject recvJson){
    QJsonObject ret;
    ret[REFID]=recvJson[REFID];
    QString token=recvJson[TOKEN].toString();
    //检查token
    if(!checkToken(socket,recvJson,MANAGER))
        return;
    ret.insert(HANDLER,"/server/retSimpleCost");
    QJsonObject dataRet;
    QJsonObject data=recvJson[DATA].toObject();
    QString id=getRoomId(recvJson);
    int idx=findRoom(id);
    if(idx<0){
        GEN_ERROR(ret,WRONG_ROOMID);
        SEND_MESSAGE(socket,ret);
        return;
    }
    AirCondition *ac=&(airConditioners[idx]);
    double money=ac->totalFee+ac->getNewFee();
    dataRet.insert("totalFee",money);
    dataRet.insert(ROOM_ID,id);
    int checkinTime=ac->openTime;
    dataRet.insert("checkinTime",checkinTime);
    int checkoutTime=getCurrentTime();
    dataRet.insert("checkoutTime",checkoutTime);
    ret.insert(DATA,dataRet);
    SEND_MESSAGE(socket,ret);
}

void WebSocketServer::detailCost(QWebSocket *socket, QJsonObject recvJson){
    QJsonObject ret;
    ret[REFID]=recvJson[REFID];
    //检查token
    if(!checkToken(socket,recvJson,MANAGER))
        return;
    ret.insert(HANDLER,"/server/retDetailCost");
    QJsonObject data=recvJson[DATA].toObject();
    QString id=getRoomId(recvJson);
    QJsonObject dataRet;
    dataRet.insert(ROOM_ID,id);
    dataRet.insert("costList",getCostListOfRoom(id));
    ret.insert("data",dataRet);
    SEND_MESSAGE(socket,ret);
}

void WebSocketServer::initRoom(QWebSocket *socket, QJsonObject recvJson)
{
    QJsonObject ret;
    ret.insert(REFID,getRefId(recvJson));
    // 解析data*/
    QJsonObject data=recvJson[DATA].toObject();
    QString roomId=getRoomId(recvJson);
    double initTmp=data["initTmp"].toDouble();
    // 该房间不存在*/
    if(airConditioners.length()==0||!findRoom(roomId)){
        AirCondition acTmp;
        acTmp.initial(roomId, initTmp);
        //test
        QString token=generateToken();
        acTmp.token=token;
        tokenMap[token]=QString("%1").arg(acTmp.roomId);
        // 将空调加入列表*/
        airConditioners.append(acTmp);
        qDebug()<<"Room:"<<QString("%1").arg(roomId)
               <<" is connected, initTmp is "<<QString("%1").arg(initTmp);
        // 加入sockMap
        sockMap[roomId] = socket;
        // 回复消息*/
        ret.insert(HANDLER,SERVER_CONFIRM);
        SEND_MESSAGE(socket,ret);
        //更新room_info表
        QString q=QString("INSERT INTO room_info values('%1','%2',%3,%4,%5)")
                .arg(roomId)
                .arg(token)
                .arg(-1)
                .arg(initTmp)
                .arg(acTmp.openTime);
        db.queryInsert(q);
        // 更新ip_info表*/
        q = QString("INSERT INTO ip_info values('%1','room',%2);")
                .arg(socket->peerAddress().toString()
                     .arg(roomId));
        db.queryInsert(q);
    }
    else{   // 回复错误信息*/
        qDebug()<<"Room:"<<QString("%1").arg(roomId)<<" is already exist.";
        ret.insert(HANDLER,SERVER_ERROR);
        ret.insert(MESSAGE,"room already exists");
        SEND_MESSAGE(socket,ret);
    }
}

void WebSocketServer::controlAc(QWebSocket *socket, QJsonObject recvJson)
{
    QJsonObject ret;
    ret.insert(REFID,getRefId(recvJson));
    QString token = recvJson[TOKEN].toString();
    // 判断该token是否存在*/
    QString id=getRoomId(recvJson);
    //检查token*/
    if(!checkToken(socket,recvJson,ROOM)){
        return;
    }

    // 找到该房间对应的空调*/
    int idx=findRoom(id);
    if(idx>=0){
        QJsonObject data=recvJson[DATA].toObject();
        double tmp = data["tmp"].toDouble();
        int wind = data["wind"].toInt();
        int power = data["power"].toInt();
        AirCondition *ac=&(airConditioners[idx]);
        if(tmp>=0){
            ac->setTmp=tmp;
        }
        if(wind!=-1){//只有风速改变需要更新总费用*/
            ac->update();
            ac->wind=wind;
        }
        if(power!=-1){
            ac->power=power;
        }
        ret.insert(HANDLER,SERVER_CONFIRM);
        SEND_MESSAGE(socket,ret);
    }else{
        GEN_ERROR(ret,WRONG_ROOMID);
        SEND_MESSAGE(socket,ret);
    }

}

void WebSocketServer::updateAc(QWebSocket *socket, QJsonObject recvJson)
{
    QJsonObject ret;
    ret.insert(REFID,getRefId(recvJson));
    QString token = recvJson[TOKEN].toString();
    // 判断该token是否存在*/
    QString id=getRoomId(recvJson);
    //检查token
    if(!checkToken(socket,recvJson,ROOM))
        return;
    // 找到该房间对应的空调*/
    int idx=findRoom(id);
    if(idx>=0){
        AirCondition *ac=&(airConditioners[idx]);
        double nowTmp = recvJson[DATA].toObject()["nowTmp"].toDouble();
        ac->nowTmp = nowTmp;

        // 构造报文*/
        ret.insert(HANDLER,"/server/updateMoney");
        ret.insert(TOKEN,token);
        QJsonObject data;
        data.insert("totalFee",ac->totalFee+ac->getNewFee());
        ret.insert(DATA,data);
        SEND_MESSAGE(socket,ret);
        /*持久化*/
        // 更新详细消费表*/
        // 更新房间状态表*/
    }
}

