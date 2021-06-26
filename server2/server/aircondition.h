#ifndef AIRCONDITION_H
#define AIRCONDITION_H

#include <QObject>
#include <QTimer>
#include <QList>
#include <QTime>

#define DISCONNECTED 0
#define CONNECTED    1  /* 也是idle状态，就是客房连接成功，但是未入住宾客 */
#define CHECKED      2  /* 非idle状态，该房间有旅客入住 */

#define REWARM_TIMEOUT 2000

class Database;

class RecordDetail
{
public:
    QString roomId;
    int requestTime;
    int requestDuration;
    int fanSpeed;
    double feeRate;
    double fee;
};

class AirCondition
{
public:

    bool idle;      /* 无人入住为True */
    bool isStart;   /* 当前是否工作(开机) */
    int mode;       /* 模式 0-制冷 1-制热，默认为制冷 */
    QString roomId;
    QString token;

    int fanSpeed;   /* 风速wind */
    double rateFeeTable[4] = {0, 0.5, 1, 2}; /* 费率表 */
    double rateFee; /* 费率 */

    double initialTemp; /* 初始温度 */
    double currentTemp; /* 当前温度 */
    double targetTemp;  /* 目标温度 */
    double lastUpdateTemp; /* 上次更新风速时的温度 */

    double currentFee;  /* 本次开机的消费 */
    double totalFee;    /* 该空调总消费 */

    QList<RecordDetail> rd; /* 该空调声明周期的详单 */
    int lastRequestTime;   /* 上次详单更新的时间 */
    double lastRequestFee;    /* 上次详单更新的消费 */
    int scheduleTimes;
    Database *db;

    explicit AirCondition();
    explicit AirCondition(int workMode, double *rateFeeTable);
    ~AirCondition();

    /* initial是客户端连接但未开房时进行的操作 */
    void initial(QString id, double nowTemp);
    void onSched(int sched);
    /* 空调开机 */
    void start(int mode, double targetTemp, double rateFeeTable[4]);
    /* 这里只是接收客户端请求后，更新空调的目标温度和风速 */
    void changeTargetTemp(double tmp);
    void changeFanSpeed(int spd);
    /* 用于帮助调度对象更新当前温度 */
    void setCurrentTemp(double tmp);
    /* 更新当前温度和累计消费 */
    double updateFee();
    /* 新产生的费用 */
    double getNewFee();
    /* 增加一条详单记录 */
    void addRD();
    /* 获得消费总费用 */
    double getTotalFee();
    /* 获得当前时间的秒数 */
    int getCurrentTime();
    /* 空调关机 */
    void end();

    void rewarming();

};

#endif // AIRCONDITION_H
