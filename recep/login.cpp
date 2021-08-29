#include "login.h"
#include "ui_login.h"

login::login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);
    init_login();
    init_connect();

}

login::~login()
{
    delete ui;
}

void login::init_login(){
    setWindowTitle("前台登录");
    setFixedSize(730,530);
    //设置窗口图标
    setWindowIcon(QIcon(":/image/title.jpg"));

}

void login::init_connect(){
    connect(ui->pushButton_login,&QPushButton::clicked,this,&login::connect_server);
    connect(reception,&QWebSocket::connected,this,&login::send_login_package);
    connect(reception,&QWebSocket::textMessageReceived,this,&login::analyze_msg);
}
void login::paintEvent(QPaintEvent *){

    QPainter painter(this);
    QPixmap pix;
    pix.load(":/image/background.jpg");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);



}

void login::connect_server(){

    url = "ws://localhost:12345";
    //reception->setParent(this);
    reception->open(QUrl(url));
    connect(reception,&QWebSocket::disconnected,[](){
        qDebug()<<"disconnected";
    });

}
void login::send_login_package(){
    QString name = ui->lineEdit_UserId->text();
    QString password = ui->lineEdit_password->text();
//    name = QString::fromStdString(MD5(name.toStdString()).toString());
//    password = QString::fromStdString(MD5(password.toStdString()).toString());
    QJsonObject json_login;
    refId = generate_refId();
    json_login.insert("refId",refId);
    json_login.insert("handler","/reception/login");
    QJsonObject data;
    data.insert("receptionId",name);
    data.insert("password",password);
    json_login.insert("data",data);
    auto jsonString = QString(QJsonDocument(json_login).toJson());
    reception->sendTextMessage(jsonString);
}
void login::analyze_msg(const QString& msg){
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(msg.toLocal8Bit().data(),&jsonError);
    if (!jsonDoc.isNull() && (jsonError.error == QJsonParseError::NoError))
    {  // 解析未发生错误
        if (jsonDoc.isObject())
        { // JSON 文档为对象
            QJsonObject json = jsonDoc.object();
            if(refIdlist.contains(json["refId"].toString()))
            {
                refIdlist.removeOne(json["refId"].toString());
                if(json["handler"]=="/server/error"){
                    if(json["msg"]== "wrong password")
                        QMessageBox::warning(this, "错误", "密码错误");
                }
                else if(json["handler"]=="/server/retRole"){
                        QJsonObject data  = json["data"].toObject();
                        QString role = data["role"].toString();
                        token = data["token"].toString();
                        if(role == "reception")
                        {
                            QMessageBox::information(this, "提示", "登陆成功！");
                            widget *w = new widget(nullptr);
                            this->close();
                            w->show();
                            disconnect(reception, &QWebSocket::textMessageReceived, this,  &login::analyze_msg);
//                            delete this;

                        }

                    }

            }

        }
    }
    else{
        qDebug()<<"json解析错误\n";
    }
}

