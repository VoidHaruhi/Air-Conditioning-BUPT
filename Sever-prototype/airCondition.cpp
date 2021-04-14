#include "airCondition.h"

AirCondition::AirCondition()
{
    power=0;
    roomId = -1;
    token = "";
    nowTmp = 25;
    defaultTmp=25;
    initTmp=25;//环境温度
    wind = 0;
    setTmp = 25;
    totalFee = 0;
    current_time = QTime::currentTime();
    runTime = -1;
    openTime = -1;
    startTime = -1;
    lastUpdateTime = -1;
    idle=true;

}

AirCondition::~AirCondition()
{
}

void AirCondition::initial(QString id, int tmp)
{
    idle=true;
    roomId = id;
    initTmp = tmp;
}

void AirCondition::open(QString systemToken,int tmp){
    token=systemToken;
    current_time = QTime::currentTime();
    openTime = current_time.hour() * 60 + current_time.minute();
    defaultTmp=tmp;
    idle=false;

}

void AirCondition::start()
{
    power=1;
    startTime = current_time.hour() * 60 + current_time.minute();
    lastUpdateTime = startTime;
    // starttime也要设置一下*/
}


void AirCondition::set(double tmp, int spd)
{
    update();
    setTmp = tmp;
    wind = spd;
}

double AirCondition::update()
{
    // 更新当前累计消费*/
    int nowTime = current_time.hour() * 60 + current_time.minute();
    int interval = nowTime - lastUpdateTime;
    double fee;
    if(wind == 1){
        fee = (double)interval / (3*60);
    }
    else if(wind == 2){
        fee = (double)interval / (2*60);
    }
    else if(wind == 3){
        fee = (double)interval / 60;
    }
    totalFee+=fee;
    lastUpdateTime = nowTime;
    return fee;
}

void AirCondition::close()
{
    update();
    power=0;
    wind = 0;
    setTmp = 25;
    runTime = -1;
    openTime = -1;
    startTime = -1;
    lastUpdateTime = -1;
}


