/**
 * @title   mng_ctrl.h
 * @module  经理控制器，负责接收者经理的任务请求，构造回复报文
 * @author  刘宇航
 * @interface   分配器声明该模块，作为控制器使用
 * @version  1.0    构造基本的经理控制器功能
 *           1.1    增加对Json包的解析功能
 */
#ifndef MNG_CTRL_H
#define MNG_CTRL_H

#include <QList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QDebug>
#include "database.h"
#include "aircondition.h"


class ManagerController
{
public:
    /**
     * @brief ManagerController 默认构造函数
     */
    ManagerController();
    /**
     * @brief db 数据库接口
     */
    Database *db;
    /**
     * @brief queryReport 查询报表功能
     * @param msg 请求报文
     * @return 回复报文
     */
    QJsonObject queryReport(QJsonObject msg);
};


#endif // MNG_CTRL_H
