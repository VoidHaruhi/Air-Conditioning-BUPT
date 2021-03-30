#include "WebSocketServer.h"
#include "ui_WebSocketServer.h"

#include <QHostAddress>
#include <QDebug>

WebSocketServer::WebSocketServer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WebSocketServer)
{
    ui->setupUi(this);

    //构造：QWebSocketServer（const QString& serverName,QWebSocketServer::SslMode secureMode,QObject *parent=nullptr）
    //使用给定的serverName构造一个新的QWebSocketServer。
    //该服务器名称将在HTTP握手阶段被用来识别服务器。它可以为空，此时不会将服务器名称发送给客户端。
    //SslMode指示服务器是通过wss（SecureMode）还是ws（NonSecureMode）运行
    //QWebSocketServer::SecureMode服务器以安全模式运行（通过wss）
    //QWebSocketServer::NonSecureMode服务器以非安全模式运行（通过ws）
    server=new QWebSocketServer("Server",QWebSocketServer::NonSecureMode,this);


    //服务器监听
    //ui->btnSend->setEnabled(false);
    //connect(ui->btnListen,&QPushButton::clicked,this,&WebSocketServer::btnListening);

    // 自动监听，没有绑定按钮和函数
    autoListening();

    //新的连接进来
    connect(server,&QWebSocketServer::newConnection,this,&WebSocketServer::onNewConnection);


    //发送消息给客户端
    connect(ui->btnSend,&QPushButton::clicked,[this](){
        if(!ui->editSend->toPlainText().isEmpty())
            emit sendMessage(ui->editSend->toPlainText());
        for (int i = clientList.length()-1; i >= 0; i--) {
            if (i % 2 == 0)
                clientList[i]->sendTextMessage("hello even*****.\n");
        }
    });
}

WebSocketServer::~WebSocketServer()
{
    clearClient();
    server->close();
    delete ui;
}


void WebSocketServer::autoListening()
{
    QHostAddress address = QHostAddress::Any;
    int port = 8080;
    if(server->listen(address, port)){
        qDebug() << "Server is listening..." << endl;
    }
    else{
        qDebug() << "*** Server is ERROR in listening. ***" << endl;
    }
}


void WebSocketServer::onNewConnection()
{
    QWebSocket *socket = server->nextPendingConnection();

    if(!socket)
        return;
    clientList.push_back(socket);

    // 在命令行输出新的连接的地址和端口号
    QString connectAddress = socket->peerAddress().toString();
    int connectPort = socket->peerPort();
    qDebug() <<"[New Connect] Address: " + connectAddress + "  Port: " + QString::number(connectPort) << endl;


    //收到消息
    connect(socket,&QWebSocket::textMessageReceived,[=](QString msg){
        //QString recvPackage = msg;
        QString connectAddress = socket->peerAddress().toString();
        int connectPort = socket->peerPort();

        while(msg != ""){
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
            msg = "";

        }
    });


//    //发送消息
//    connect(this,&WebSocketServer::sendMessage,socket,&QWebSocket::sendTextMessage);
//    //断开连接，释放
//    connect(socket,&QWebSocket::disconnected,[this,socket](){
//        clientList.removeAll(socket);
//        socket->deleteLater();
//    });
}

void WebSocketServer::packageAnalyse(QWebSocket *socket, QJsonObject recvJson)
{
    int request = recvJson["request"].toInt();

    switch (request)
    {
    case 1:
    {
        qDebug() <<"request = 1" << endl;
        socket->sendTextMessage("Success test!");
        break;
    }
    default:
    {
        qDebug() <<"Json包请求码错误." << endl;
        break;
    }

    }
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
