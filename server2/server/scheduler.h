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
#define STATE_SET 0
#define STATE_READY 1
#define TIME_SLOT 1/*每隔多少时间进行一次超时检查*/
#define TIME_SLICE 10/*等待时间片*/
#define TEMP_CHANGE 0.05 /*每个回温时间片的温度变化量*/


class Scheduler:public QObject{
    Q_OBJECT
public:
    Scheduler(QWebSocketServer* sv,std::map<QString,QWebSocket*> *sp);


    /*向空调发送调风速消息
    * 找到对应的空调，修改其风速
    */
    void changeFanSpeedOfAc(QString roomId,int fanSpeed);
    /*轮询回温*/
    void rewarm();
    void changeAc(QWebSocket *sock,int power,double tmp,int wind);

    /*供dispatcher调用*/
    /* 管理员控制器 */
    QJsonObject managerAnalyse(QJsonObject msg);
    /* 空调控制器 */
    QJsonObject acAnalyse(QJsonObject msg);
    /*roodId和房间websocket的字典，用于转发*/
    std::map<QString,QWebSocket*> *sockMap;
    QWebSocketServer *server;
private:
    /*管理员*/
    /*设置空调运行参数（中央空调）*/
    bool setPara(int mode,double tempLimits[2],double defaultTargetTemp,double feeRateTable[4]);
    /*空调启动*/
    bool startUp();
    /*空调*/
    /*空调开机*/
    bool powerOn(QString roomId,double currentTemp);
    /*空调关机*/
    bool powerOff(QString roomId);
    /*开启空调*/
    bool startAc(QString roomId,int mode,double targetTemp,double rateFeeTable[4],double currentFee,double totalFee);
    /*更改目标温度*/
    bool changeTargetTemp(QString roomId,double targetTemp);
    /*更改风速*/
    bool changeFanSpeed(QString roomId,int fanSpeed);
    /*升温*/
    bool requestTempUp(QString roomId);
    /*取消升温*/
    bool requestRelease(QString roomId);
    /*调度*/
    /*更换等待队列中的服务*/
    bool changeServiceInWaitQueue(QString oldId,QString newId,int fanSpeed,int waitTime);
    /*更改等待队列中的服务*/
    bool modifyWaitQueue(QString roomId,int fanSpeed,int waitTime);
    /*在服务队列中查找房间*/
    bool searchRoomService(QString roomId);
    /*在等待队列中查找房间*/
    bool searchRoomWait(QString roomId);
    /*更换服务队列中的服务*/
    bool changeServiceInServiceQueue(QString oldId,QString newId,int fanSpeed,int waitTime);
    /*更改服务队列中的服务*/
    bool modifyServiceQueue(QString roomId,int fanSpeed,int startTime);
    /*与服务队列中的服务比较风速*/
    QString compareFanSpeedService(int fanSpeed);
    /*与等待队列中的服务比较风速*/
    QString compareFanSpeedWait(int fanSpeed);
    /*服务加入等待队列*/
    bool addToWaitQueue(QString roomId,int fanSpeed,int waitTime);
    /*服务加入服务队列*/
    bool addToServiceQueue(QString roomId,int fanSpeed,int startTime);
    /*检查是否有*/
    void checkTimeout();
    /*对超时的等待服务进行调度*/
    bool schedWait(QString roomId,int fanSpeed);
    /*新来一个请求的时候，对应powerOn*/
    bool schedNew(QString roomId,int fanSpeed);
    /*change类*/
    bool schedChange(QString roomId,int fanSpeed);
    /*服务队列中有服务被删除*/
    bool schedDel();
    /**/
    AirCondition *getAc(QString roomId);

    /**/
    int defaultMode;
    /**/
    double defaultTargetTemp;
    /*空调默认开启风速*/
    int defaultFanSpeed;
    /**/
    double defaultTempLimit[2];
    /**/
    double rateFeeTable[4];
    /**/
    int state;
    /**/
    ServiceQueue *serviceQueue;
    /**/
    WaitQueue *waitQueue;
    /**/
    QTimer *timer;
    /**/
    QTimer *timerWork;
    /**/
    QList<AirCondition*> airConditioners;
};


#endif // SCHEDULER_H
