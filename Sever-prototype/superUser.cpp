#include "superUser.h"

SuperUser::SuperUser()
{
    username = "null";
    password = "null";
    token = "null";
    role = "MANAGER";
}

SuperUser::~SuperUser()
{

}

// 注册
QString SuperUser::logon(QString uname, QString upassword, int type)
{
    username = uname;
    password = upassword;
    if(type == MANAGER){
        // 查询数据库 是否重名

        return "succeeded";
    }
    else if(type == ADMINISTRATOR){
        // 查询数据库 是否重名

        return "succeeded";
    }
    return "failed";
}

// 登录
QString SuperUser::login(QString uname, QString upassword, int type)
{
    username = uname;
    password = upassword;
    if(type == MANAGER){
        // 查询数据库 用户是否存在，密码是否正确

        return "succeeded";
    }
    else if(type == ADMINISTRATOR){
        // 查询数据库 用户是否存在，密码是否正确

        return "succeeded";
    }
    return "failed";
}

void SuperUser::setToken(QString systemtoken)
{
    token = systemtoken;
}

// 登出
QString SuperUser::logout()
{
    username = "";
    password = "";
    token = "";

    return "succeeded";
}
