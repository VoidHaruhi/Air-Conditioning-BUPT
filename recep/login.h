/**
 * @title login.h
 * @author 唐方柳
 */
#ifndef LOGIN_H
#define LOGIN_H

#include "head.h"
#include "widget.h"



QT_BEGIN_NAMESPACE
namespace Ui { class login; }
QT_END_NAMESPACE

/**
 * @brief The login class 前台登录
 * @author 唐方柳
 */
class login : public QWidget
{
    Q_OBJECT

public:
    QString url;
    login(QWidget *parent = nullptr);
    /**
     * @brief init_login 初始化登录界面
     */
    void init_login();
    /**
     * @brief init_connect 初始化信号和槽
     */
    void init_connect();

    ~login();

private slots:
    /**
     * @brief connect_server 连接服务器

     */
    void connect_server();
    /**
     * @brief send_login_package 发送登录包
     */
    void send_login_package();
    /**
     * @brief analyze_msg 解析服务器返回登录包
     * @param msg 接受到的数据包
     */
    void analyze_msg(const QString& msg);
private:
    Ui::login *ui;
    /**
     * @brief paintEvent 画背景图
     */
    void paintEvent(QPaintEvent *);


};
#endif // LOGIN_H
