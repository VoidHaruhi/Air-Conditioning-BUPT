#include "mng_ctrl.h"
#include "database.h"
#include "header.h"
#include <QTime>

ManagerController::ManagerController()
{
    db = new Database;
}



QJsonObject ManagerController::queryReport(QJsonObject msg)
{
    QJsonObject answer;
    QStringList request = msg["handler"].toString().split('/');
    answer.insert(REFID,msg[REFID]);


    if(request[2] == "login"){
        QJsonObject data = msg[DATA].toObject();
        QString pwd = data["password"].toString();
        if(pwd == PASSWORD){
            answer.insert(HANDLER,"/server/retRole");
            QJsonObject retData;
            retData.insert("role","manager");
            retData.insert("token","nothing");
            answer.insert(DATA, retData);
        }
        else{
            answer.insert(HANDLER,"/server/error");
            answer.insert("msg", "wrong password");
        }
    }
    else if(request[2] == "openRoom"){
        answer.insert(HANDLER,"/server/confirm");
    }
    else if(request[2] == "queryreport"){
        QJsonObject retData;
        answer.insert("handler", "/server/report");

        QList<QString> reportList = db->dbReportQuery();
        QJsonArray retList;
        for(int i=0; i<reportList.size(); i++){
            QStringList result = reportList[i].split('/');
            QJsonObject item;
            item.insert("roomId", result[0]);
            item.insert("switchTimes", result[1].toInt());
            item.insert("scheduleTimes", result[2].toInt());
            item.insert("rdCount", result[3].toInt());
            item.insert("tempChangeTimes", result[4].toInt());
            item.insert("spdChangeTimes", result[5].toInt());
            item.insert("requestTime", result[6].toDouble());
            item.insert("totalFee", result[7].toDouble());
            retList.append(item);
        }

        retData.insert("reportList", retList);
        answer.insert("data", retData);

    }
    else{
        answer.insert(HANDLER, "/server/error");
    }
    return answer;
}
