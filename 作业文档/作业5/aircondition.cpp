#include "aircondition.h"
#include "headers.h"
AirCondition::AirCondition()
{
    mode = 0;   /* 默认制冷 */
    roomId = -1;
    token = "";

    initialTemp = 24;
    currentTemp = 24;
    targetTemp = 24;
    lastUpdateTemp = 24;

    fanSpeed = 0;
    rateFee = rateFeeTable[fanSpeed];
    totalFee = currentFee = 0;

    idle = true;
    isStart = false;

    rd.clear();
}

AirCondition::AirCondition(int workMode, double *rateFeeTable)
{
    idle = true;
    isStart = false;

    mode = workMode;   /* 0-制冷 1-制热 */
    roomId = -1;
    token = "";

    initialTemp = 24;
    currentTemp = 24;
    targetTemp = 24;
    lastUpdateTemp = 24;

    /* 费率表由管理员设置 */
    for(int i=0; i<4; i++){
        this->rateFeeTable[i] = rateFeeTable[i];
    }

    fanSpeed = 0;
    rateFee = rateFeeTable[fanSpeed];
    totalFee = currentFee = 0;

    rd.clear();
}

AirCondition::~AirCondition()
{
}

void AirCondition::initial(QString id, double nowTemp)
{
    roomId = id;
    currentTemp = nowTemp;
}

void AirCondition::open(QString systemToken){
    token = systemToken;
    idle = false;   /* 该空调已被插卡，未启动 */
}

void AirCondition::start()
{
    initialTemp = currentTemp;
    lastUpdateTemp = currentTemp;
    fanSpeed = 1;   /* 默认是低速风 */
    rateFee = rateFeeTable[fanSpeed];
    lastRequestTime = getCurrentTime();
    lastRequestFee = 0;
    isStart = true;
}


void AirCondition::changeFanSpeed(int spd)
{
    updateFee();
    addRD();
    fanSpeed = spd;
    rateFee = rateFeeTable[fanSpeed];
}

void AirCondition::changeTargetTemp(double tmp)
{
    targetTemp = tmp;
}

void AirCondition::setCurrentTemp(double tmp)
{
    currentTemp = tmp;
}

double AirCondition::getNewFee()
{
    if(mode == 0){  /* 制冷模式温度变化是负数 */
        return (lastUpdateTemp - currentTemp) * rateFee;
    }
    else if(mode == 1){
        return (currentTemp - lastUpdateTemp) * rateFee;
    }
}

double AirCondition::updateFee()
{
    // 更新当前累计消费*/
    double fee = getNewFee();
    totalFee += fee;
    currentFee += fee;
    lastUpdateTemp = currentTemp;
    return fee;
}

void AirCondition::addRD()
{
    RecordDetail rdItem;
    rdItem.roomId = this->roomId;
    rdItem.requestTime = this->lastRequestTime;
    lastRequestTime = getCurrentTime();
    rdItem.requestDuration = lastRequestTime - rdItem.requestTime;
    rdItem.fanSpeed = this->fanSpeed;
    rdItem.feeRate = this->rateFee;
    rdItem.fee = currentFee - lastRequestFee;
    lastRequestFee = currentFee;
    rd.append(rdItem);
}

double AirCondition::getTotalFee()
{
    return totalFee;
}

int AirCondition::getCurrentTime()
{
    QTime current_time = QTime::currentTime();
    int nowTime = (current_time.hour()*3600 + current_time.minute()*60 + current_time.second());
    return nowTime;
}

void AirCondition::end()
{
    updateFee();
    addRD();
    isStart = false;
    fanSpeed = 0;
    rateFee = rateFeeTable[fanSpeed];
	currentFee = 0;

}


