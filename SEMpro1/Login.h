#ifndef Login_H
#define Login_H
#include<QTcpSocket>
#include <QWidget>
#include <QtNetwork/QTcpServer>
#include"widget.h"
#include"header.h"
#include<iostream>
using namespace std;
class Signup;
namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();
    void Iniconnect();//初始化信号和槽
    void IniRegi();//初始化登陆界面
    void keyPressEvent(QKeyEvent *event);//
    void sendloginpak(QString name,QString pass);


private:
    Ui::Login *ui;
    QWebSocket* client;
    void connectSrv();
    QString refid="";
//    QTcpSocket *Client;
private slots:
    void Login_Pressed();
    void Connected();
    void Signup_Pressed();
    void recv_msg(const QString& msg);
};

#endif // Login_H
