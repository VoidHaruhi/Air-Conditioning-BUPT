#ifndef AIRCONDITION_H
#define AIRCONDITION_H

#include<QString>
#include <QTime>

#define DISCONNECTED 0
#define CONNECTED 1        /*// 也是idle状态，就是客房连接成功，但是未入住宾客*/
#define CHECKED 2          /*// 非idle状态，该房间有旅客入住*/

class AirCondition
{
public:
    int power;
    int roomId;
    QString token;
    int wind;
    double nowTmp;
    double setTmp;/*// 目标温度*/
    double totalFee;

    QTime current_time;
    int runTime;    /*// 目前没用上*/
    int openTime;
    int startTime;
    int defaultTmp;
    int lastUpdateTime;
    bool idle;/*//无人入住为True*/

    AirCondition();
    ~AirCondition();

    // initial是客户端连接但未开房时进行的操作*/
    void initial(int id, int tmp);
    // open是开房操作，收到管理员开房通知后的操作*/
    void open(QString systemToken,int tmp);
    // 空调开机*/
    void start();
    // 这里只是接收客户端请求后，更新空调的目标温度和风速*/
    void set(int tmp, int spd);
    // 随着服务器时间更新当前温度和累计消费*/
    void update();
    // 空调关机*/
    void close();


};

#endif // AIRCONDITION_H
