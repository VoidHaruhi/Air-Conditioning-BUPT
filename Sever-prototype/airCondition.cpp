#include "airCondition.h"

AirCondition::AirCondition()
{
    power=0;
    roomId = -1;
    token = "null";
    defaultTmp = 25;
    nowTmp = 25;
    wind = 0;
    wind = 0;
    setTmp = 25;
    totalFee = 0;
    runTime = 0;
    idle=true;
}

AirCondition::~AirCondition()
{

}

void AirCondition::initial(int id, int tmp)
{
    idle=false;
    roomId = id;
    initTmp = tmp;


}
void AirCondition::open(QString systemToken){
    token=systemToken;
}
void AirCondition::start()
{
    power=1;
    // starttime也要设置一下
}


void AirCondition::set(int tmp, int spd)
{
    setTmp = tmp;
    wind = spd;
}

void AirCondition::update()
{
    // 更新当前温度
    // 更新当前累计消费
}

void AirCondition::close()
{
    power=0;
}


