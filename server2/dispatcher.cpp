/**
 * @title   dispatcher.cpp
 * @module  实现分配器类中的方法，主要实现通信模块
 * @author  刘宇航
 * @interface   主函数直接声明该模块，作为总的通信接口
 * @version  1.0    创建Websocket通信模块
 *           1.1    添加Json包解析功能
 *           1.2    添加控制器对象
 */
#include "dispatcher.h"
#include <QCryptographicHash>
#include <QRandomGenerator64>
Dispatcher::Dispatcher(QObject *parent):
    QObject(parent)
{
    server=new QWebSocketServer("Server",QWebSocketServer::NonSecureMode,this);
    scheduler=new Scheduler(server,&sockMap);
    /* 初始化经理和管理员控制器 */
    receptionController=new ReceptionController();
    managerController=new ManagerController();
    QHostAddress address=QHostAddress::Any;
    /* 初始化数据库对象 */
    db = new Database;
    db->dbDropAll();
    db->dbInitial();
    /* 开始监听 */
    int port;
    port=12345;
    if(!server->listen(address,port)){
        qDebug()<<"Listen error";

    }else{
        qDebug()<<"Listening";
    }
    /* 将新连接的信号绑定到onNewConnection槽中 */
    connect(server,&QWebSocketServer::newConnection,this,&Dispatcher::onNewConnection);
}

Dispatcher::~Dispatcher(){
    server->close();
}

void Dispatcher::onNewConnection(){
    QWebSocket *socket=server->nextPendingConnection();
    if(!socket)
        return;
    /* 套接字加入客户端队列 */
    clientList.push_back(socket);
    /* 收到消息 */
    connect(socket,&QWebSocket::textMessageReceived,[=](const QString &msg){
        QString connectAddress = socket->peerAddress().toString();
        int connectPort = socket->peerPort();
            /* 进行包的解析 */
            QByteArray recvPackage = msg.toUtf8();
            QJsonParseError parseJsonErr;
            QJsonDocument document = QJsonDocument::fromJson(recvPackage, &parseJsonErr);
            if(!(parseJsonErr.error == QJsonParseError::NoError))
            {
                qDebug()<<"解析json文件错误.";
                return;
            }
            QJsonObject msg1=document.object();
            QJsonObject ret = dispatch(socket, msg1);
            if(msg1[HANDLER]!="/client/confirm")
                SEND_MESSAGE(socket, ret);
            //emit process(socket,document.object());

    });
    /* 断开连接，释放 */
    connect(socket,&QWebSocket::disconnected,[this, socket](){
        clientList.removeAll(socket);
        socket->deleteLater();
    });
}

QJsonObject Dispatcher::dispatch(QWebSocket *socket, QJsonObject msg)
{
    QStringList request = msg[HANDLER].toString().split('/');
    QJsonObject answer;
    if(request[2]!="requestState"){
        qDebug()<<request[1]<<" "<<request[2];
    }
    /* 管理员&经理发来消息 */
    if (request[1]=="manager"){
        if(request[2]=="login"||request[2]=="queryreport"){
            /* 经理控制器 */
            answer = managerController->queryReport(msg);
        }
        /* 前台开房，向空调发送开房信息 */
        else if(request[2]=="openRoom"){
            QString tok=generateToken();
            tokenMap[tok]="client";
            QJsonObject ret;
            ret.insert(REFID,msg[REFID]);
            ret.insert(HANDLER,"/server/openRoom");
            QJsonObject data;
            data.insert(TOKEN,tok);
            ret.insert("data",data);
            /* 空调控制器 */
            QJsonObject acret = scheduler->acAnalyse(ret);
            QString id=getRoomId(msg);
            SEND_MESSAGE(sockMap[id],acret);

            /* 管理员控制器 */
            answer = managerController->queryReport(msg);
        }
        else{
            /* 管理员控制器 */
            answer = scheduler->managerAnalyse(msg);
        }
    }
    /* 前台发来消息 */
    else if (request[1]=="reception"){
        /* 前台控制器 */
        answer = receptionController->analyse(msg);

    }
    /* 客户端发来消息 */
    else if(request[1]=="client"){
        bool auth=true;
        if(request[2]=="confirm"){
            QJsonObject ret;
            ret.insert(REFID,msg[REFID]);
            ret.insert(HANDLER,"/server/confirm");
            answer=ret;
            return answer;
        }
        if(request[2]!="init"){
            answer = scheduler->acAnalyse(msg);
        }
        else if(request[2]=="init"){
            /* 建立socket映射 */
            QJsonObject data = msg[DATA].toObject();
            QString roomId = data["roomId"].toString();
            sockMap[roomId] = socket;
            /* 空调控制器 */
            answer = scheduler->acAnalyse(msg);
        }

    }
    else{
        GEN_ERROR(answer, "No Such Role");
    }
    return answer;
}

QString Dispatcher::generateToken(){
    /* 生成token */
    QString salt=QTime::currentTime().toString("zzz")+QString("%1").arg(QRandomGenerator::global()->bounded(10000000));
    QString token=QCryptographicHash::hash(salt.toLatin1(),QCryptographicHash::Md5).toHex();
    qDebug()<<"Token generated"<<token<<salt;
    return token;
}

QString Dispatcher::generateMD5(QString str){
    return QCryptographicHash::hash(str.toLatin1(),QCryptographicHash::Md5).toHex();
}

bool Dispatcher::checkToken(QWebSocket *sock, QJsonObject recvJson, QString role){
    QString token=getToken(recvJson);
    bool flag=true;
    QJsonObject ret;
    /* 检查是否是合法token */
    if(tokenMap.count(token)==0){
        qDebug()<<"Wrong token "<<token << role;
        GEN_ERROR(ret,WRONG_TOKEN);
        flag=false;
    }
    else if(tokenMap[token]!=role){
        qDebug()<<"Wrong role "<<token << role;
        GEN_ERROR(ret,WRONG_ROLE);
        flag=false;
    }
    if(!flag)
        SEND_MESSAGE(sock,ret);
    return flag;
}

QString Dispatcher::getToken(QJsonObject j){
    if(!j.contains("token")){
        qDebug()<<"No field token";
        return "null";
    }
    return j["token"].toString();
}

QString Dispatcher::getRoomId(QJsonObject j){
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
