#include "airCondition.h"

AirCondition::AirCondition()
{
    power=0;
    roomId = -1;
    token = "null";
    nowTmp = 25;
    defaultTmp=25;
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

void AirCondition::initial(int id, int tmp)
{
    idle=true;
    roomId = id;
    nowTmp = tmp;
}

void AirCondition::open(QString systemToken,int tmp){
    token=systemToken;
    openTime = current_time.hour() * 3600 + current_time.minute() * 60 + current_time.second();
    defaultTmp=tmp;
    idle=false;

}

void AirCondition::start()
{
    power=1;
    startTime = current_time.hour() * 3600 + current_time.minute() * 60 + current_time.second();
    lastUpdateTime = startTime;
    // starttime也要设置一下*/
}


void AirCondition::set(int tmp, int spd)
{
    update();
    setTmp = tmp;
    wind = spd;
}

void AirCondition::update()
{
    // 更新当前累计消费*/
    int nowTime = current_time.hour() * 3600 + current_time.minute() * 60 + current_time.second();
    int interval = nowTime - lastUpdateTime;
    if(wind == 1){
        totalFee += (double)interval / (3*60);
    }
    else if(wind == 2){
        totalFee += (double)interval / (2*60);
    }
    else if(wind == 3){
        totalFee += (double)interval / 60;
    }
    lastUpdateTime = nowTime;

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


