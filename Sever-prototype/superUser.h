#ifndef SUPERUSER_H
#define SUPERUSER_H
#include<QString>
#define ADMINISTRATOR 1
#define MANAGER       0

class SuperUser
{
public:
    QString username;
    QString password;
    QString token;
    QString role;

    SuperUser();
    ~SuperUser();
    // 注册 可能会返回失败信息
    QString logon(QString uname, QString upassword, int type);
    // 登录
    QString login(QString uname, QString upassword, int type);
    // 设置口令
    void setToken(QString systemtoken);
    // 登出
    QString logout();
};

#endif // SUPERUSER_H
