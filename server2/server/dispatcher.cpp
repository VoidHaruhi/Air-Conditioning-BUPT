#include "dispatcher.h"
#include <QCryptographicHash>
#include <QRandomGenerator64>
Dispatcher::Dispatcher(QObject *parent):
    QObject(parent)
{
    server=new QWebSocketServer("Server",QWebSocketServer::NonSecureMode,this);
    scheduler=new Scheduler(server,&sockMap);
    receptionController=new ReceptionController();
    managerController=new ManagerController();
    QHostAddress address=QHostAddress::Any;

    db = new Database;
    db->dbDropAll();
    db->dbInitial();

    int port;
    port=12345;
    if(!server->listen(address,port)){
        qDebug()<<"Listen error";

    }else{
        qDebug()<<"Listening";
    }

    connect(server,&QWebSocketServer::newConnection,this,&Dispatcher::onNewConnection);

    //connect(this,&Dispatcher::process,this,&Dispatcher::packageAnalyse);
}

Dispatcher::~Dispatcher(){
    server->close();
}

void Dispatcher::onNewConnection(){
    QWebSocket *socket=server->nextPendingConnection();
    if(!socket)
        return;
    clientList.push_back(socket);//套接字加入客户端队列*/
    //收到消息*/
    connect(socket,&QWebSocket::textMessageReceived,[=](const QString &msg){
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
            QJsonObject ret = dispatch(socket, document.object());
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
    qDebug()<<request;

    if (request[1]=="manager"){ /* 管理员&经理发来消息 */
        if(request[2]=="login"||request[2]=="queryreport"){
            /* 经理控制器 */
            answer = managerController->queryReport(msg);
        }
        else if(request[2]=="openRoom"){/*前台开房，向空调发送开房信息*/
            QString tok=generateToken();
            tokenMap[tok]="client";
            QJsonObject ret;
            ret.insert(REFID,answer[REFID]);
            ret.insert(HANDLER,"server/openRoom");
            QJsonObject data;
            data.insert(TOKEN,tok);
            ret.insert("data",data);

            /* 空调控制器 */
            QJsonObject acret = scheduler->acAnalyse(ret);
            SEND_MESSAGE(sockMap[getRoomId(msg)],acret);

            /* 管理员控制器 */
            answer = managerController->queryReport(msg);
        }
        else{
            /* 管理员控制器 */
            answer = scheduler->managerAnalyse(msg);
        }
    }
    else if (request[1]=="reception"){ /* 前台发来消息 */
        /* 前台控制器 */
        answer = receptionController->analyse(msg);

    }
    else if(request[1]=="client"){
        bool auth=true;
        if(request[2]!="init"){
            /* 检查token */
            if(sockMap[getRoomId(msg)])
                auth=checkToken(sockMap[getRoomId(msg)],msg,"client");
            else{
                qDebug()<<"Room not found in sockMap";
                GEN_ERROR(answer, "No Room Socket");
            }
            /* token正确 */
            if(auth){
                /* 空调控制器 */
                answer = scheduler->acAnalyse(msg);
            }

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
