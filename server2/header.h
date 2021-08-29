/**
 * @title   head.h
 * @module  宏定义的头文件
 * @author  于越
 * @interface   一些常用的宏定义供各模块调用
 * @version  1.0    定义一些常用的宏
 */

#ifndef HEADER_H
#define HEADER_H

/* 以下宏用于构造和解析报文 */
#define WRONG_TOKEN  "wrong token"
#define WRONG_ROOMID "wrong roomId"
#define WRONG_ROLE   "wrong role"
#define NULLSTRING   "null"
#define HANDLER      "handler"
#define REFID        "refId"
#define DATA         "data"
#define TOKEN        "token"
#define DEFAULT_TMP  "defaultTmp"
#define MANAGER      "manager"
#define ADMIN        "admin"
#define ROOM         "room"
#define ROOM_ID      "roomId"
#define CONFIRM      "confirm"
#define MESSAGE      "msg"
#define SERVER_ERROR    "/server/error"
#define SERVER_CONFIRM  "/server/confirm"

/* 管理员、经理、前台的初始密码 */
#define PASSWORD "111111"

/* 加密的宏 */
#define MD5PASSQORD(x) (QCryptographicHash::hash(x.toLatin1(),QCryptographicHash::Md5).toHex())

/* 发送报文的宏 */
#define SEND_MESSAGE(sock, jsn) {QString _msg = QString(QJsonDocument(jsn).toJson());\
    if(sock->sendTextMessage(_msg) != _msg.length())\
    {qDebug() << "Send Error!";}\
    }

#include<QObject>
#include<QDebug>
#endif // HEADER_H
