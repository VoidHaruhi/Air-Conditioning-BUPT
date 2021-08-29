/**
 * @title sched_queue.h
 * @module 调度队列模块，分为等待队列和服务队列
 * @author 于越
 * @interface 提供等待队列和服务队列的接口，供调度器进行调用
 * @version v1.0 编写了基类BasicQueue, 衍生类ServiceQueue和WaitQueue
 */
#ifndef SCHED_QUEUE_H
#define SCHED_QUEUE_H

#include <QVector>
#include <QTimer>
#include <QString>
#define MAX_SERVICE 3
/**
 * @brief The QueueType class 队列服务对象
 */
class QueueType{
public:
    /*
     * 构造函数
     */
    QueueType(QString id, int speed, int wt, int st);
    QueueType();
    /**
     * @brief roomId 房间号
     */
    QString roomId;
    /**
     * @brief fanSpeed 风速
     */
    int fanSpeed;
    /**
     * @brief waitTime 等待时间（时间片，服务对象没有）
     */
    int waitTime;
    /**
     * @brief startTime 服务开始时间（等待对象没有）
     */
    int startTime;
    /**
     * @brief printQ 打印队列对象
     */
    void printQ();
};
/**
 * @brief The BasicQueue class 队列类的基类
 */
class BasicQueue{
public:
    BasicQueue();
    /**
     * @brief insert 向队列插入一个对象
     * @param q 队列对象
     */
    void insert(QueueType q);
    /**
     * @brief remove 从队列中删除一个对象，以roomId为索引
     * @param id 房间号
     * @return 被移除的对象
     */
    QueueType remove(QString id);
    /**
     * @brief update 更新队列
     * @param q 更新的对象
     */
    void update(QueueType q);
    /**
     * @brief getSpeed 获取房间的风速
     * @param roomId 房间号
     * @return 风速
     */
    int getSpeed(QString roomId);
    /**
     * @brief exists 判断roomId在队列中是否存在
     * @param roomId 房间号
     * @return 存在返回True，不存在返回False
     */
    bool exists(QString roomId);
    /**
     * @brief roomList 房间列表
     */
    QVector<QueueType> roomList;
};
/**
 * @brief The WaitQueue class
 */
class WaitQueue:public BasicQueue{
public:
    /*
     * 函数名：checkTimeout
     * 功能：将队列中的等待时间减一个固定值，如果时间片耗尽则需要进行调度
     * 参数：过去了多少时间
     * 返回值：一列服务
     */
    QVector<QString> checkTimeout(int timepassed);
    /**
     * @brief modify 修改等待对象
     * @param roomId 房间号
     * @param fanSpeed 新风速
     * @param waitTime 新等待时间片
     * @return 是否成功
     */
    bool modify(QString roomId,int fanSpeed,int waitTime);
    /**
     * @brief change 替换等待对象
     * @param oldId 被替换的房间号
     * @param newId 新的房间号
     * @param fanSpeed 新风速
     * @param waitTime 新等待时间片
     * @return 是否成功
     */
    bool change(QString oldId,QString newId,int fanSpeed,int waitTime);
    /**
     * @brief add 添加等待对象
     * @param roomId 房间号
     * @param fanSpeed 新风速
     * @param waitTime 等待时间片
     * @return 是否成功
     */
    bool add(QString roomId,int fanSpeed,int waitTime);
    /**
     * @brief compare 将风速与等待队列中的风速进行比较
     * @param fanSpeed 风速
     * @return 结果为小，返回none；否则返回最适合的时间片剩余最小的对象的房间号
     */
    QString compare(int fanSpeed);

private:
    /**
     * @brief waitTimer 等待队列计时器
     */
    QTimer waitTimer;
};
/**
 * @brief The ServiceQueue class
 */
class ServiceQueue:public BasicQueue{
public:
    /**
     * @brief modify 更改服务对象
     * @param roomId 房间号
     * @param fanSpeed 风速
     * @param startTime 服务开始时间
     * @return 是否修改成功
     */
    bool modify(QString roomId,int fanSpeed,int startTime);
    /**
     * @brief change 替换服务对象
     * @param oldId 原房间号
     * @param newId 新房间号
     * @param fanSpeed 风速
     * @param startTime 服务开始时间
     * @return 是否替换成功
     */
    bool change(QString oldId,QString newId,int fanSpeed,int startTime);
    /**
     * @brief add 添加服务对象
     * @param roomId 房间号
     * @param fanSpeed 风速
     * @param startTime 开始时间
     * @return 是否添加成功
     */
    bool add(QString roomId,int fanSpeed,int startTime);
    /**
     * @brief compare 与服务队列中的对象进行比较，优先级
     * @param fanSpeed 风速
     * @return 比较结果（见实现，由两部分组成），为"small"或"equal"或"big"+被替换房间号
     */
    QString compare(int fanSpeed);
    /**
     * @brief choose 选择一个服务时间最长的对象换出
     * @return 被换出的房间号
     */
    QString choose();
};

#endif // SCHED_QUEUE_H
