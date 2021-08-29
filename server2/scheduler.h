/**
 * @title scheduler.h
 * @module 调度控制器，由空调控制器、管理员控制器、调度员共同组成（为了简化结构）
 * @author 于越
 * @interface dispather调用该模块，进行解析，执行相应动作
 * @version v1.0 实现基本功能
 *          v2.0 在基础上实现了调度，进行debug
 */
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <QString>
#include <QTimer>
#include <QTimerEvent>
#include <QTime>
#include <QObject>
#include <QList>
#include <cmath>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonDocument>
#include<QWebSocket>
#include<QWebSocketServer>
#include "aircondition.h"
#include "sched_queue.h"


#define MY_MINUTE 60
#define STATE_SET 0
#define STATE_READY 1
#define TIME_SLOT 1/*每隔多少时间进行一次超时检查*/
#define TIME_SLICE 2*MY_MINUTE/*等待时间片*/
#define TIME_MAX 0x7fffffff-1

/**
 * @brief The Scheduler class 调度+空调控制器+管理员控制器
 */
class Scheduler:public QObject{
    Q_OBJECT
public:
    Scheduler(QWebSocketServer* sv,std::map<QString,QWebSocket*> *sp);
    /**
     * @brief changeFanSpeedOfAc 向空调发送调风速消息，找到对应的空调，修改其风速
     * @param roomId
     * @param fanSpeed
     */
    void changeFanSpeedOfAc(QString roomId,int fanSpeed);
    /**
     * @brief changeAc 调空调的风速
     * @param sock socket
     * @param power 电源
     * @param tmp 目标温度
     * @param wind 风速
     */
    void changeAc(QWebSocket *sock,int power,double tmp,int wind);
    /**
     * @brief managerAnalyse 管理员控制器，供dispatcher调用
     * @param msg 待解析的消息
     * @return 解析返回的报文
     */
    QJsonObject managerAnalyse(QJsonObject msg);
    /* 空调控制器 */
    /**
     * @brief acAnalyse 空调控制器，供dispatcher调用
     * @param msg 待解析的消息
     * @return 解析返回的报文
     */
    QJsonObject acAnalyse(QJsonObject msg);
    /**
     * @brief sockMap roodId和房间websocket的字典，用于转发
     */
    std::map<QString,QWebSocket*> *sockMap;
    /**
     * @brief server Websocket服务器
     */
    QWebSocketServer *server;
private:
    /*管理员*/
    /*设置空调运行参数（中央空调）*/
    /**
     * @brief setPara 设定中央空调参数
     * @param mode 运行模式，制冷/制热
     * @param tempLimits 温度下限和上限
     * @param defaultTargetTemp 默认目标温度
     * @param feeRateTable 费率表（已作废）
     * @return 是否设置成功
     */
    bool setPara(int mode,double tempLimits[2],double defaultTargetTemp,double feeRateTable[4]);
    /*空调启动*/
    /**
     * @brief startUp 启动空调
     * @return 是否启动成功
     */
    bool startUp();
    /*空调*/
    /**
     * @brief powerOn 开房
     * @param roomId 房间号
     * @param currentTemp 房间当前温度
     * @return 是否开房成功
     */
    bool powerOn(QString roomId,double currentTemp);
    /**
     * @brief powerOff 空调关机
     * @param roomId 房间号
     * @return 是否关机成功
     */
    bool powerOff(QString roomId);
    /**
     * @brief startAc 开启空调
     * @param roomId 房间号
     * @param mode 工作模式
     * @param targetTemp 目标温度
     * @param rateFeeTable 费率表
     * @param currentFee 当前费用（从上一次开机开始）
     * @param totalFee 总费用（当前房客）
     * @return 是否开机成功
     */
    bool startAc(QString roomId,int mode,double targetTemp,double rateFeeTable[4],double currentFee,double totalFee);
    /**
     * @brief changeTargetTemp 更改目标温度
     * @param roomId 房间号
     * @param targetTemp 目标温度
     * @return 是否成功
     */
    bool changeTargetTemp(QString roomId,double targetTemp);
    /**
     * @brief changeFanSpeed 更改风速
     * @param roomId 房间号
     * @param fanSpeed 风速
     * @return 是否成功
     */
    bool changeFanSpeed(QString roomId,int fanSpeed);

    /*调度*/
    /**
     * @brief changeServiceInWaitQueue 更换等待队列中的服务
     * @param oldId 原房间号
     * @param newId 新房间号
     * @param fanSpeed 风速
     * @param waitTime 等待时间
     * @return 是否成功
     */
    bool changeServiceInWaitQueue(QString oldId,QString newId,int fanSpeed,int waitTime);
    /**
     * @brief modifyWaitQueue 更改等待队列中的服务
     * @param roomId 房间号
     * @param fanSpeed 风速
     * @param waitTime 等待时间
     * @return 是否成功
     */
    bool modifyWaitQueue(QString roomId,int fanSpeed,int waitTime);
    /**
     * @brief searchRoomService 在服务队列中查找房间
     * @param roomId 房间号
     * @return 查找结果
     */
    bool searchRoomService(QString roomId);
    /**
     * @brief searchRoomWait 在等待队列中查找房间
     * @param roomId 房间号
     * @return 查找结果
     */
    bool searchRoomWait(QString roomId);
    /**
     * @brief changeServiceInServiceQueue 更换服务队列中的服务
     * @param oldId 房间号
     * @param newId 房间号
     * @param fanSpeed 风速
     * @param waitTime 等待时间片
     * @return 是否成功更改
     */
    bool changeServiceInServiceQueue(QString oldId,QString newId,int fanSpeed,int waitTime);
    /**
     * @brief modifyServiceQueue 更改服务队列中的服务
     * @param roomId 房间号
     * @param fanSpeed 风速
     * @param startTime 服务开始时间
     * @return 是否成功更改
     */
    bool modifyServiceQueue(QString roomId,int fanSpeed,int startTime);
    /**
     * @brief compareFanSpeedService 与服务队列中的服务比较风速
     * @param fanSpeed 风速
     * @return 详见serviceQueue的compare
     */
    QString compareFanSpeedService(int fanSpeed);
    /**
     * @brief compareFanSpeedWait 与等待队列中的服务比较风速
     * @param fanSpeed 风速
     * @return 详见waitQueue的compare
     */
    QString compareFanSpeedWait(int fanSpeed);
    /*服务加入等待队列*/
    /**
     * @brief addToWaitQueue
     * @param roomId 房间号
     * @param fanSpeed
     * @param waitTime
     * @return 是否成功加入
     */
    bool addToWaitQueue(QString roomId,int fanSpeed,int waitTime);
    /**
     * @brief addToServiceQueue 服务加入服务队列
     * @param roomId 房间号
     * @param fanSpeed 风速
     * @param startTime 服务开始时间
     * @return 是否成功加入
     */
    bool addToServiceQueue(QString roomId,int fanSpeed,int startTime);
    /**
     * @brief checkTimeout 检查是否有等待对象超时
     */
    void checkTimeout();
    /**
     * @brief schedWait 对超时的等待服务进行调度
     * @param roomId 房间号
     * @param fanSpeed
     * @return 是否成功调度
     */
    bool schedWait(QString roomId,int fanSpeed);
    /**
     * @brief schedNew 新来一个请求的时候（等待队列改变也算作新请求）
     * @param roomId 房间号
     * @param fanSpeed 风速
     * @return 是否成功调度
     */
    bool schedNew(QString roomId,int fanSpeed);
    /**
     * @brief schedChange 有风速变化时的调度
     * @param roomId 房间号
     * @param fanSpeed 风速
     * @return 是否成功调度
     */
    bool schedChange(QString roomId,int fanSpeed);
    /**
     * @brief schedDel 服务对象删除时的调度
     * @return 是否成功调度
     */
    bool schedDel();
    /**
     * @brief getAc通过房号获取空调
     * @param roomId 房间号
     * @return
     */
    AirCondition *getAc(QString roomId);

    /**
     * @brief defaultMode 默认工作模式
     */
    int defaultMode;
    /**
     * @brief defaultTargetTemp 默认目标温度
     */
    double defaultTargetTemp;
    /**
     * @brief defaultFanSpeed 默认风速
     */
    int defaultFanSpeed;
    /**
     * @brief defaultTempLimit 默认温度限制
     */
    double defaultTempLimit[2];
    /**
     * @brief rateFeeTable 费率表
     */
    double rateFeeTable[4];
    /**
     * @brief state 中央空调状态
     */
    int state;
    /**
     * @brief startTime 开始时间
     */
    int startTime;
    /**
     * @brief serviceQueue 服务队列
     */
    ServiceQueue *serviceQueue;
    /**
     * @brief waitQueue 等待队列
     */
    WaitQueue *waitQueue;
    /**
     * @brief timer 检查队列超时的定时器
     */
    QTimer *timer;
    /**
     * @brief timerWork 空调工作定时器
     */
    QTimer *timerWork;
    /**
     * @brief airConditioners 空调列表
     */
    QList<AirCondition*> airConditioners;
};


#endif // SCHEDULER_H
