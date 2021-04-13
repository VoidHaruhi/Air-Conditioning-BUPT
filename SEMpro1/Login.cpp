#include "Login.h"
#include "ui_Login.h"

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);


    IniRegi();
    Iniconnect();


}
void Login::IniRegi()
{
    setWindowTitle("宠物小精灵对战登录端");
    setFixedSize(800,600);
    this->setAutoFillBackground(true);
//    QPalette palette = this->palette();
//    palette.setBrush(QPalette::Background, QBrush(QPixmap(":res/regi.jpg")));
//    this->setPalette(palette);
    ui->password_edit->setEchoMode(QLineEdit::Password);
    ui->password_edit->setPlaceholderText("请输入密码");
    ui->name_edit->setPlaceholderText("请输入用户名");
    ui->login_btn->setDisabled(true);
    ui->name_edit->setDisabled(true);
    ui->password_edit->setDisabled(true);

//    //登录
//    connect(ui->login_btn, &QPushButton::clicked, [=](){

//    });

}
void Login::connectSrv()
{
    QString path = QString("ws://%1:%2").arg(QString(SRVERADDR), QString(SRVERPORT));
    QUrl url = QUrl(path);

    client->open(url);
    connect(client,SIGNAL(connected()),this,SLOT(so_connected()));
}
void Login::Iniconnect()
{
     connect(ui->login_btn, SIGNAL(clicked()), this, SLOT(Login_Pressed()));
//     connect(ui->signup_btn, SIGNAL(clicked()), this, SLOT(Signup_Pressed()));
     client = new QWebSocket();
     connect(client, &QWebSocket::connected, this, &Login::Connected);
     connect(client, &QWebSocket::textMessageReceived, this,  &Login::recv_msg);
     connectSrv();
}
void Login::sendloginpak(QString name,QString pass)
{
//    name = QString::fromStdString(MD5(name.toStdString()).toString());
//    pass = QString::fromStdString(MD5(pass.toStdString()).toString());
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
void Login::Connected()//连接到服务器 退出登陆界面 跳转主界面
{

}
void Login::Login_Pressed()
{
    QString uname = ui->name_edit->text();
    QString password = ui->password_edit->text();
    if(!uname.isEmpty() && !password.isEmpty())
    {
        sendloginpak(uname,password);
    }

}
void Login::Signup_Pressed()
{


}
void Login::recv_msg(const QString& msg)
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
                if(json[HANDLER]=="/server/error"){
                    if(json[MESSAGE]== "wrong password")
                        QMessageBox::warning(this, "错误", "密码错误");
                }
                else if(json[HANDLER]=="/server/retRole"){
                        QJsonObject data  = json[DATA].toObject();
                        QString role = data[ROLE].toString();
                        QString token = data[TOKEN].toString();
                        if(role == "manager")
                        {
                            QMessageBox::information(this, "提示", "登陆成功！");
                            Widget *si = new Widget(token,nullptr,1);
                            this->close();
                            si->show();
                            disconnect(client, &QWebSocket::textMessageReceived, this,  &Login::recv_msg);
//                            delete this;

                        }
                        else{
                        }

                    }

            }

        }
    }

}
void Login::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter)
        {
          Login_Pressed();
           return;
        }
}
void Login::so_connected()
{
    ui->login_btn->setEnabled(true);
    ui->name_edit->setEnabled(true);
    ui->password_edit->setEnabled(true);
}
Login::~Login()
{
    delete ui;
}


