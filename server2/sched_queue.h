#ifndef SCHED_QUEUE_H
#define SCHED_QUEUE_H
#include <QVector>
#include <QTimer>
#include <QString>
#define MAX_SERVICE 3
class QueueType{
public:
    QueueType(QString id, int speed, int wt, int st);
    QueueType();
    QString roomId;
    int fanSpeed;
    int waitTime;
    int startTime;
    void printQ();
};

class BasicQueue{
public:
    BasicQueue();
    void insert(QueueType q);
    QueueType remove(QString id);
    void update(QueueType q);
    int getSpeed(QString roomId);
    bool exists(QString roomId);
    QVector<QueueType> roomList;
};

class WaitQueue:public BasicQueue{
public:
    /*
     * 函数名：checkTimeout
     * 功能：将队列中的等待时间减一个固定值，如果时间片耗尽则需要进行调度
     * 参数：过去了多少时间
     * 返回值：一列服务
     */
    QVector<QString> checkTimeout(int timepassed);
    bool modify(QString roomId,int fanSpeed,int waitTime);
    bool change(QString oldId,QString newId,int fanSpeed,int waitTime);
    bool add(QString roomId,int fanSpeed,int waitTime);
    /* 用来判断是否需要与新服务替换
     * high:返回"none", low/equal: 返回roomId*/
    QString compare(int fanSpeed);

private:
    QTimer waitTimer;
};
class ServiceQueue:public BasicQueue{
public:

    bool modify(QString roomId,int fanSpeed,int startTime);
    bool change(QString oldId,QString newId,int fanSpeed,int startTime);
    bool add(QString roomId,int fanSpeed,int startTime);
    /* 用来判断是否需要与一个到时间了的等待服务替换
     * low:返回"none", equal/high: 返回roomId*/
    QString compare(int fanSpeed);
};

#endif // SCHED_QUEUE_H
