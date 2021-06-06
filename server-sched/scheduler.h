#ifndef SCHEDULER_H
#define SCHEDULER_H

#include<QString>
#include<QTimer>
#include<QTimerEvent>
#include<QTime>
#include<QObject>
#include "sched_queue.h"
#include "aircondition.h"
#define STATE_SET 0
#define STATE_READY 1
#define TIME_SLOT 1/*每隔多少时间进行一次超时检查*/
#define TIME_SLICE 10/*等待时间片*/
class Scheduler:public QObject{
    Q_OBJECT
public:
    Scheduler();
    bool changeServiceInWaitQueue(QString oldId,QString newId,int fanSpeed,int waitTime);
    bool modifyWaitQueue(QString roomId,int fanSpeed,int waitTime);
    bool searchRoomService(QString roomId);
    bool searchRoomWait(QString roomId);
    bool changeServiceInServiceQueue(QString oldId,QString newId,int fanSpeed,int waitTime);
    bool modifyServiceQueue(QString roomId,int fanSpeed,int startTime);
    QString compareFanSpeedService(int fanSpeed);
    QString compareFanSpeedWait(int fanSpeed);
    bool addToWaitQueue(QString roomId,int fanSpeed,int waitTime);
    bool addToServiceQueue(QString roomId,int fanSpeed,int startTime);
    void checkTimeout();
    /*对超时的等待服务进行调度*/
    bool schedWait(QString roomId,int fanSpeed);
    /*新来一个请求的时候，对应powerOn*/
    bool schedNew(QString roomId,int fanSpeed);
    /*change类*/
    bool schedChange(QString roomId,int fanSpeed);
    bool setPara(int mode,double tempLimits[2],double defaultTargetTemp,double feeRateTable[4]);
    bool startUp();

    bool powerOn(QString roomId,double currentTemp);
    bool powerOff(QString roomId);
    bool startAc(int mode,double targetTemp,double rateFeeTable[4],double currentFee,double totalFee);
    bool changeTargetTemp(QString roomId,double targetTemp);
    bool changeFanSpeed(QString roomId,int fanSpeed);
    bool requestTempUp(QString roomId);
    bool requestRelease(QString roomId);
    /*checkRoomState你设计一下*/

    /*向空调发送调风速消息
    * 找到对应的空调，修改其风速
    */
    void changeFanSpeedOfAc(QString roomId,int fanSpeed);

private:
    int defaultMode;
    double defaultTargetTemp;
    /*空调默认开启风速*/
    int defaultFanSpeed;
    double defaultTempLimit[2];
    double rateFeeTable[4];
    int state;
    ServiceQueue *serviceQueue;
    WaitQueue *waitQueue;
    QTimer *timer;
    QList<AirCondition*> airConditioners;
};


#endif // SCHEDULER_H
