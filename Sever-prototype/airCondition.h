#ifndef AIRCONDITION_H
#define AIRCONDITION_H

#include<QString>

#define DISCONNECTED 0
#define CONNECTED 1        /* 也是idle状态，就是客房连接成功，但是未入住宾客*/
#define CHECKED 2          /* 非idle状态，该房间有旅客入住*/

class AirCondition
{
public:
    int power;
    int roomId;
    QString token;
    double defaultTmp;
    double initTmp;
    int wind;
    double nowTmp;
    double setTmp;/* 目标温度*/
    double totalFee;
    int runTime;
    int startTime;
    bool idle;/*无人入住为True*/

    AirCondition();
    ~AirCondition();

/*     initial是客户端连接但未开房时进行的操作，空调开机*/
    void initial(int id, int tmp);
    /* open是开房操作，收到管理员开房通知后的操作*/
    void open(QString systemToken);
    void start();
    /* 这里只是接收客户端请求后，更新空调的目标温度和风速 */
    void set(int tmp, int spd);
    /* 随着服务器时间更新当前温度和累计消费*/
    void update();
    /* 这里不是断开连接，而是退房了，空调没有关机*/
    void close();


};

#endif // AIRCONDITION_H
