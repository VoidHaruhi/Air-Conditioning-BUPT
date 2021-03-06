/**
 * @title   aircondition.cpp
 * @module  实现空调（服务）对象的类中方法
 * @author  刘宇航
 * @interface   调度模块应该在房间连接时创建服务对象，服务对象作为房间空调的模型进行调度和服务
 * @version  1.0    创建空调对象类的基本信息
 *           1.1    添加计费功能
 *           1.2    增加对数据库的访问
 */
#include "aircondition.h"
#include "database.h"
#include<stdio.h>
#include<math.h>

AirCondition::AirCondition()
{
    mode = 0;   /* 默认制冷 */
    roomId = "";
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

    db = new Database;
}

AirCondition::~AirCondition()
{
}

void AirCondition::initial(QString id, double nowTemp)
{
    roomId = id;
    currentTemp = nowTemp;
    initialTemp = nowTemp;
    lastUpdateTemp=nowTemp;
    /* 该空调已被插卡，未启动 */
    idle = false;
    startTime=getCurrentTime();
    tempup=false;
    /* 修改账单表 */
    QString q = QString("insert into invoice values('%1', %2)")
            .arg(this->roomId)
            .arg(0);
    db->dbInsert(q);
    /* 修改报表表 */
    q = QString("insert into report values('%1',0,0,0,0,0,0,0)")
                .arg(this->roomId);
    db->dbInsert(q);
}

void AirCondition::start(int mode,double targetTemp,double rateFeeTable[4])
{
    this->mode = mode;
    this->targetTemp = targetTemp;
    for(int i=0; i<4; i++){
        this->rateFeeTable[i] = rateFeeTable[i];
    }
    rateFee = rateFeeTable[fanSpeed];
    lastRequestTime = getCurrentTime();
    lastRequestFee = 0;
    isStart = true;
    startTime=getCurrentTime();
    /* 修改报表中的开关次数 */
    db->dbUpdate(QString("update report set switchTimes=switchTimes+1 where roomId='%1'").arg(this->roomId));
}


void AirCondition::changeFanSpeed(int spd)
{
    updateFee();
    if(spd==0){
        isStart=false;
    }else{
        isStart=true;
    }
    if(fanSpeed != 0)
        addRD();
    else{
        lastRequestTime = getCurrentTime();
    }
    fanSpeed = spd;
    rateFee = rateFeeTable[fanSpeed];
    /* 修改报表中的调风次数 */
    db->dbUpdate(QString("update report set spdChangeTimes=spdChangeTimes+1 where roomId='%1'").arg(this->roomId));
}

void AirCondition::changeTargetTemp(double tmp)
{
    targetTemp = tmp;
    /* 修改报表中的调温次数 */
    db->dbUpdate(QString("update report set tempChangeTimes=tempChangeTimes+1 where roomId='%1'").arg(this->roomId));
}

void AirCondition::setCurrentTemp(double tmp)
{
    currentTemp = tmp;
    if(this->isStart==true){
        updateFee();
    }
}

double AirCondition::getNewFee()
{
    return fabs(lastUpdateTemp-currentTemp);
}

double AirCondition::updateFee()
{
    double fee = getNewFee();
    /* 更新当前累计消费*/
    lastUpdateTemp = currentTemp;
    if(!isStart){
        return 0;
    }
    totalFee += fee;
    currentFee += fee;
    return fee;
}

void AirCondition::addRD()
{
    RecordDetail rdItem;
    rdItem.roomId = this->roomId;
    rdItem.requestTime = this->lastRequestTime;
    this->lastRequestTime = getCurrentTime();
    rdItem.requestDuration = this->lastRequestTime - rdItem.requestTime;
    rdItem.fanSpeed = this->fanSpeed;
    rdItem.feeRate = this->rateFee;
    rdItem.fee = currentFee - lastRequestFee;
    lastRequestFee = currentFee;
    rd.append(rdItem);

    /* 向详单做插入操作 */
    QString q = QString("insert into rd values('%1', %2, %3, %4, %5, %6)")
            .arg(rdItem.roomId)
            .arg(rdItem.requestTime)
            .arg(rdItem.requestDuration)
            .arg(rdItem.fanSpeed)
            .arg(1.0)
            .arg(rdItem.fee);
    db->dbInsert(q);
    q = QString("update invoice set totalFee=%1 where roomId = '%2'")
            .arg(this->totalFee)
            .arg(this->roomId);
    db->dbUpdate(q);

    /* 修改报表中的详单条目数 */
    db->dbUpdate(QString("update report set rdCount=rdCount+1 where roomId='%1'").arg(this->roomId));
    /* 修改报表中的总费用 */
    db->dbUpdate(QString("update report set totalFee=%1 where roomId='%2'")
                 .arg(this->totalFee)
                 .arg(this->roomId));
    /* 修改报表中的服务时长 */
    db->dbUpdate(QString("update report set requestTime=requestTime+%1 where roomId='%2'")
                 .arg(rdItem.requestDuration)
                 .arg(this->roomId));

}

double AirCondition::getTotalFee()
{
    return totalFee;
}

int AirCondition::getCurrentTime()
{
    QTime current_time = QTime::currentTime();
    int nowTime = current_time.hour()*3600 + current_time.minute()*60 + current_time.second();
    return nowTime;
}

void AirCondition::end()
{
    updateFee();
    addRD();
    isStart = false;
    tempup=false;
    fanSpeed = 0;
    rateFee = rateFeeTable[fanSpeed];
    currentFee = 0;

}
void AirCondition::onSched(int sched){
    /* 修改报表中的调度次数 */
    db->dbUpdate(QString("update report set scheduleTimes=scheduleTimes+%1 where roomId='%2'").arg(sched).arg(this->roomId));
}

