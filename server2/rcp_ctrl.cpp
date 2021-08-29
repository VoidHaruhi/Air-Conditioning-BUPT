/**
 * @title   rcp_ctrl.cpp
 * @module  实现前台控制器类中的方法
 * @author  刘宇航
 * @interface   分配器声明该模块，作为控制器使用
 * @version  1.0    构造基本的前台控制器功能
 *           1.1    增加对Json包的解析功能
 */
#include "rcp_ctrl.h"
#include "database.h"
#include "header.h"

ReceptionController::ReceptionController()
{
    db = new Database;
}

double ReceptionController::createInvoice(QString roomId)
{
    double fee = 0;
    /* 查询该房间的消费金额 */
    QString query = QString("select * from rd where roomId = '%1'").arg(roomId);
    rd = db->dbRDQuery(query);
    for(int i=0; i<rd.size(); i++){
        fee += rd[i].fee;
    }
    return fee;
}

QList<RecordDetail> ReceptionController::createRD(QString roomId)
{
    /* 查询该房间的详单列表 */
    QString query = QString("select * from rd where roomId = '%1'").arg(roomId);
    rd = db->dbRDQuery(query);
    return rd;
}

QJsonObject ReceptionController::analyse(QJsonObject msg)
{
    QJsonObject answer;
    answer.insert(REFID, msg[REFID]);
    QStringList request = msg[HANDLER].toString().split('/');
    /* 登录请求 */
    if(request[2] == "login"){
        QJsonObject data = msg[DATA].toObject();
        QString pwd = data["password"].toString();
        if(pwd == PASSWORD){
            answer.insert(HANDLER,"/server/retRole");
            QJsonObject retData;
            retData.insert("role","reception");
            retData.insert("token","nothing");
            answer.insert(DATA, retData);
        }
        else{
            answer.insert(HANDLER,"/server/error");
            answer.insert("msg", "wrong password");
        }
    }
    /* 查询账单请求 */
    else if(request[2] == "simpleCost"){
        QJsonObject data = msg[DATA].toObject();
        QString id = data["roomId"].toString();
        answer.insert("handler", "/server/retSimpleCost");
        QJsonObject retData;
        retData.insert("roomId", id);
        double fee = createInvoice(id);
        retData.insert("fee", fee);
        answer.insert("data", retData);
    }
    /* 查询详单请求 */
    else if(request[2] == "detailCost"){
        QJsonObject data = msg[DATA].toObject();
        QString id = data["roomId"].toString();
        answer.insert("handler", "/server/retDetailCost");
        QJsonObject retData;
        retData.insert("roomId", id);
        QList<RecordDetail> rd = createRD(id);
        QJsonArray retList;
        for(int i=0; i<rd.size(); i++){
            QJsonObject item;
            item.insert("roomId", rd[i].roomId);
            item.insert("requestTime", rd[i].requestTime);
            item.insert("requestDuration", rd[i].requestDuration);
            item.insert("fanSpeed", rd[i].fanSpeed);
            item.insert("feeRate", rd[i].feeRate);
            item.insert("fee", rd[i].fee);
            retList.append(item);
        }
        retData.insert("roomList", retList);
        answer.insert("data", retData);
    }
    /* 其他，回复错误报文 */
    else{
        answer.insert(HANDLER, "/server/error");
    }
    return answer;
}

