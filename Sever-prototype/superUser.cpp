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

// 登录

void SuperUser::setToken(QString systemtoken)
{
    token = systemtoken;
}

// 登出*/
QString SuperUser::logout()
{
    username = "";
    password = "";
    token = "";

    return "succeeded";
}
