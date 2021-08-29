/**
 * @title   rcp_ctrl.h
 * @module  前台控制器，负责接收者前台的任务请求，构造回复报文
 * @author  刘宇航
 * @interface   分配器声明该模块，作为控制器使用
 * @version  1.0    构造基本的前台控制器功能
 *           1.1    增加对Json包的解析功能
 */
#ifndef RCP_CTRL_H
#define RCP_CTRL_H

#include <QList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QDebug>
#include "aircondition.h"
#include "database.h"

class ReceptionController
{
public:
    /**
     * @brief ReceptionController 默认构造函数
     */
    ReceptionController();
    /**
     * @brief db 数据库访问接口
     */
    Database *db;
    /**
     * @brief rd 详单列表
     */
    QList<RecordDetail> rd;
    /**
     * @brief analyse 包解析函数
     * @param msg 请求Json报文
     * @return 回复Json报文
     */
    QJsonObject analyse(QJsonObject msg);
    /**
     * @brief createInvoice 查询账单
     * @param roomId 房间号
     * @return 房间总费用
     */
    double createInvoice(QString roomId);
    /**
     * @brief createRD 查询详单
     * @param roomId 房间号
     * @return 详单列表
     */
    QList<RecordDetail> createRD(QString roomId);
};


#endif // RCP_CTRL_H
