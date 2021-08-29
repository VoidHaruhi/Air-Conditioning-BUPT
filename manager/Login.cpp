#include "Login.h"
#include "ui_Login.h"
/**
* @projectName   SEMpro2
* @brief         程序运行首先生成登陆界面
* @author        Oliver
* @module        初始化登陆界面，连接服务端，输入用户名密码，发送登陆请求报文
* @date          2021-06-30
*/
Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);


    IniRegi();
    Iniconnect();


}
void Login::IniRegi()//初始化界面大小，title  设置界面中图形化控件默认状态 内容
{
    setWindowTitle("Super-Users For ACsystem");
    setFixedSize(500,440);
    this->setAutoFillBackground(true);
    ui->password_edit->setEchoMode(QLineEdit::Password);
    ui->password_edit->setPlaceholderText("Please input password:");
    ui->name_edit->setPlaceholderText("Login as:");
    ui->login_btn->setDisabled(true);
    ui->name_edit->setDisabled(true);
    ui->password_edit->setDisabled(true);
    ui->concel_btn->setDisabled(true);


}
void Login::connectSrv()//连接服务器
{
    QString path = QString("ws://%1:%2").arg(QString(SRVERADDR), QString(SRVERPORT));
    QUrl url = QUrl(path);

    client->open(url);
    connect(client,SIGNAL(connected()),this,SLOT(so_connected()));
}
void Login::Iniconnect()//连接信号与槽函数
{
     connect(ui->login_btn, SIGNAL(clicked()), this, SLOT(Login_Pressed()));
     client = new QWebSocket();
     connect(client, &QWebSocket::textMessageReceived, this,  &Login::recv_msg);
     connectSrv();
}
void Login::sendloginpak(QString name,QString pass)//发送登陆请求包
{
    QJsonObject json;
    refid = generate_refId();
    json[REFID] = refid;
    json[HANDLER] = "/manager/login";
    QJsonObject data;
    data[ADMINID] = name;
    data[PASSWORD] = pass;
    json[DATA] = data;
    auto jsonString = QString(QJsonDocument(json).toJson());
    client->sendTextMessage(jsonString);
}
void Login::Login_Pressed()//登陆按钮按下对应的槽函数 调用sendloginpak
{
    QString uname = ui->name_edit->text();
    QString password = ui->password_edit->text();
    if(!uname.isEmpty() && !password.isEmpty())
    {
        sendloginpak(uname,password);
    }

}

void Login::recv_msg(const QString& msg)//对收到的所有报文进行解析 并对每一种情况作出判断 实施对应的动作
{

    //解包
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(msg.toLocal8Bit().data(),&jsonError);
    if (!jsonDoc.isNull() && (jsonError.error == QJsonParseError::NoError))
    {  // 解析未发生错误
        if (jsonDoc.isObject())
        { // JSON 文档为对象
            QJsonObject json = jsonDoc.object();
            if(refIdlsit.contains(json[REFID].toString()))
            {
                refIdlsit.removeOne(json[REFID].toString());
                if(json[HANDLER]=="/server/error"){//密码错误收到错误包 显示给用户
                    if(json[MESSAGE]== "wrong password")
                        QMessageBox::warning(this, "错误", "密码错误");
                }
                else if(json[HANDLER]=="/server/retRole"){
                        QJsonObject data  = json[DATA].toObject();
                        QString role = data[ROLE].toString();
                        QString token = data[TOKEN].toString();
                        if(role == "manager"||role == "admin")//成功登陆 打开主界面
                        {
                            QMessageBox::information(this, "提示", "登陆成功！");
                            Widget *si = new Widget(token,nullptr,1);
                            this->close();
                            si->show();
                            disconnect(client, &QWebSocket::textMessageReceived, this,  &Login::recv_msg);

                        }
                    }
            }

        }
    }

}
void Login::keyPressEvent(QKeyEvent *event)//按下Enter键即发送登陆包
{
    if(event->key() == Qt::Key_Enter)
        {
          Login_Pressed();
           return;
        }
}
void Login::so_connected()//如果正确连接服务端，所有控件回复可用状态
{
    ui->login_btn->setEnabled(true);
    ui->name_edit->setEnabled(true);
    ui->password_edit->setEnabled(true);
    ui->concel_btn->setEnabled(true);
}
Login::~Login()
{
    delete ui;
}



void Login::on_concel_btn_clicked()
{
    ui->password_edit->clear();
    ui->name_edit->clear();
}
