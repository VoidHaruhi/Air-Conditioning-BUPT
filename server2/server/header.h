#ifndef HEADER_H
#define HEADER_H

#define WRONG_TOKEN  "wrong token"
#define WRONG_ROOMID "wrong roomId"
#define WRONG_ROLE   "wrong role"

#define NULLSTRING   "null"

#define HANDLER      "handler"
#define REFID        "refId"
#define DATA         "data"
#define TOKEN        "token"
#define DEFAULT_TMP  "defaultTmp"
#define MANAGER      "manager"   /*管理员*/
#define ADMIN        "admin"
#define ROOM         "room"
#define ROOM_ID      "roomId"
#define CONFIRM      "confirm"
#define MESSAGE      "msg"

#define ONE_MINUTE 10

#define SERVER_ERROR    "/server/error"
#define SERVER_CONFIRM  "/server/confirm"

#define PASSWORD "111111"
#define MD5PASSQORD(x) (QCryptographicHash::hash(x.toLatin1(),QCryptographicHash::Md5).toHex())
#define SEND_MESSAGE(sock, jsn) {QString _msg = QString(QJsonDocument(jsn).toJson());\
    if(sock->sendTextMessage(_msg) != _msg.length())\
    {qDebug() << "Send Error!";}\
     qDebug() <<_msg;}
#endif // HEADER_H
