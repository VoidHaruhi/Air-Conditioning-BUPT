/**
 * @title   dispatcher.h
 * @module  分配器模块，负责接收通信消息，并将任务分配给各个控制器
 * @author  刘宇航
 * @interface   主函数直接声明该模块，作为总的通信接口
 * @version  1.0    创建Websocket通信模块
 *           1.1    添加Json包解析功能
 *           1.2    添加控制器对象
 */
#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <QList>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QDebug>
#include <iostream>
#include <map>
#include "header.h"
#include "aircondition.h"
#include "rcp_ctrl.h"
#include "mng_ctrl.h"
#include "scheduler.h"
#include "database.h"

/* 发送错误报文的宏 */
#define GEN_ERROR(ret,tok) {ret.insert("handler",SERVER_ERROR);ret.insert("msg",tok);}


class Dispatcher:public QObject{
    Q_OBJECT

public:
    /**
     * @brief Dispatcher 显示构造函数
     * @param parent 继承QObject类
     */
    explicit Dispatcher(QObject *parent = nullptr);
    ~Dispatcher();
private:
    /**
     * @brief server QWebSocket对象指针
     */
    QWebSocketServer *server;
    /**
     * @brief clientList 客户端socket列表
     */
    QList<QWebSocket*> clientList;
    /**
     * @brief receptionController 前台控制器对象
     */
    ReceptionController *receptionController;
    /**
     * @brief managerController 经理控制器对象
     */
    ManagerController *managerController;
    /**
     * @brief scheduler 调度器对象
     */
    Scheduler *scheduler;
    /**
     * @brief db 数据库接口
     */
    Database *db;

    /**
     * @brief onNewConnection 监听新的socket连接的槽函数
     */
    void onNewConnection();
    /**
     * @brief dispatch 分配各个任务给控制器
     * @param socket 任务发起方的socket
     * @param msg 请求Json报文
     * @return 回复Json报文
     */
    QJsonObject dispatch( QWebSocket *socket, QJsonObject msg);
    /**
     * @brief generateToken 生成口令
     * @return 口令
     */
    static QString generateToken();
    /**
     * @brief checkToken 检查口令是否有效
     * @param sock 任务发起方的socket
     * @param recvJson 收到的Json报文
     * @param role 任务发起方的角色
     * @return 是有是有效token
     */
    bool checkToken(QWebSocket *sock,QJsonObject recvJson, QString role);
    /**
     * @brief generateMD5 MD5加密
     * @param str 待加密的内容
     * @return 加密后的字符串
     */
    static QString generateMD5(QString str);
    /**
     * @brief getToken 获取token
     * @param j 请求Json报文
     * @return 角色
     */
    static QString getToken(QJsonObject j);
    /**
     * @brief getRoomId 获取房间号
     * @param j 请求Json报文
     * @return 房间号
     */
    static QString getRoomId(QJsonObject j);
    /**
     * @brief sockMap roodId和房间websocket的字典，用于转发
     */
    std::map<QString,QWebSocket*> sockMap;
    /**
     * @brief tokenMap role:管理员为manager，经理为admin，房间为房间号
     */
    std::map<QString,QString> tokenMap;

};

#endif // DISPATCHER_H
