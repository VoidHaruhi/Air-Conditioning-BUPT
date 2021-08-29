/**
 * @title   aircondition.h
 * @module  声明空调（服务）对象的类，针对每一个客户端空调提供服务并记录服务和空调信息
 * @author  刘宇航
 * @interface   调度模块应该在房间连接时创建服务对象，服务对象作为房间空调的模型进行调度和服务
 * @version  1.0    创建空调对象类的基本信息
 *           1.1    添加计费功能
 *           1.2    增加对数据库的访问
 */
#ifndef AIRCONDITION_H
#define AIRCONDITION_H

#include <QObject>
#include <QTimer>
#include <QList>
#include <QTime>

/**
 *  为了防止交叉引用头文件，声明一个数据库访问类
 */
class Database;

/**
 * @brief The RecordDetail class 详单类，存储详单信息
 */
class RecordDetail
{
public:
    QString roomId;         /* 房间号 */
    int requestTime;        /* 请求开始时间 */
    int requestDuration;    /* 请求时长 */
    int fanSpeed;           /* 风速 */
    double feeRate;         /* 费率 */
    double fee;             /* 费用 */
};

class AirCondition
{
public:

    bool idle;                  /* 无人入住为True */
    bool isStart;               /* 当前是否工作(开机) */
    int mode;                   /* 模式 0-制冷 1-制热，默认为制冷 */
    QString roomId;             /* 房间号 */
    QString token;              /* 口令 */

    int fanSpeed;               /* 风速 */
    double rateFeeTable[4] = {0, 0.5, 1, 2}; /* 费率表 */
    double rateFee;             /* 当前费率 */

    double initialTemp;         /* 初始环境温度 */
    double currentTemp;         /* 当前温度 */
    double targetTemp;          /* 目标温度 */
    double lastUpdateTemp;      /* 上次计算费用时的温度 */

    double currentFee;          /* 本次开机的消费 */
    double totalFee;            /* 该空调总消费 */

    QList<RecordDetail> rd;     /* 该空调工作周期的详单 */
    int lastRequestTime;        /* 上次详单更新的时间 */
    double lastRequestFee;      /* 上次详单更新的消费 */
    int scheduleTimes;          /* 调度次数 */
    int state;                  /* 当前状态 */

    int startTime;              /* 启动时间 */
    bool tempup;                /* 是否处于回温状态 */

    /**
     * @brief db 访问数据库的接口
     */
    Database *db;
    /**
     * @brief AirCondition 默认构造函数，采用制冷模式
     */
    explicit AirCondition();
    /**
     * @brief AirCondition 显示构造函数，由调度器调用
     * @param workMode  工作模式，制冷or制热
     * @param rateFeeTable  费率表
     */
    explicit AirCondition(int workMode, double *rateFeeTable);
    ~AirCondition();
    /**
     * @brief initial 客户端空调连接但未开房时进行的操作，获得房间初始温度
     * @param id 房间号
     * @param nowTemp 房间初始温度
     */
    void initial(QString id, double nowTemp);
    /**
     * @brief onSched 修改调度次数
     * @param sched  sql语句的参数
     */
    void onSched(int sched);
    /**
     * @brief start 空调开机，开始工作
     * @param mode 模式
     * @param targetTemp 目标温度
     * @param rateFeeTable 费率表
     */
    void start(int mode, double targetTemp, double rateFeeTable[4]);
    /**
     * @brief changeTargetTemp 修改目标温度
     * @param tmp 目标温度
     */
    void changeTargetTemp(double tmp);
    /**
     * @brief changeFanSpeed 修改风速
     * @param spd 风速
     */
    void changeFanSpeed(int spd);
    /**
     * @brief setCurrentTemp 修改更新当前温度
     * @param tmp 当前温度
     */
    void setCurrentTemp(double tmp);
    /**
     * @brief updateFee 更新消费金额
     * @return 此段时间内的消费金额
     */
    double updateFee();
    /**
     * @brief getNewFee 获取此段时间的消费金额
     * @return 消费金额
     */
    double getNewFee();
    /**
     * @brief addRD 增加一条详单记录
     */
    void addRD();
    /**
     * @brief getTotalFee 获得总消费金额
     * @return 总消费金额
     */
    double getTotalFee();
    /**
     * @brief getCurrentTime 获得当前时间
     * @return 当前时间
     */
    int getCurrentTime();
    /**
     * @brief end 空调关机，停止计费
     */
    void end();
    /**
     * @brief rewarming 回温
     */
    void rewarming();

};

#endif // AIRCONDITION_H
