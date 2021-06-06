#include "scheduler.h"

Scheduler::Scheduler(){
    serviceQueue=new ServiceQueue();
    waitQueue=new WaitQueue();
    airConditioners.clear();
    timer=new QTimer();
    timer->setInterval(1000*TIME_SLOT);
    connect(timer,&QTimer::timeout,this,&Scheduler::checkTimeout);
}
bool Scheduler::setPara(int mode, double *tempLimits, double defaultTargetTemp, double *feeRateTable){
    this->defaultMode=mode;
    this->defaultTargetTemp=defaultTargetTemp;
    defaultFanSpeed=1;
    memcpy(this->defaultTempLimit,tempLimits,sizeof(defaultTempLimit));
    memcpy(this->rateFeeTable,feeRateTable,sizeof(rateFeeTable));
    state=STATE_SET;
    return true;
}
bool Scheduler::startUp(){
    state=STATE_READY;
    return true;
}
//wait
bool Scheduler::changeServiceInWaitQueue(QString oldId, QString newId, int fanSpeed, int waitTime){
    return waitQueue->change(oldId,newId,fanSpeed,waitTime);
}
bool Scheduler::modifyWaitQueue(QString roomId, int fanSpeed, int waitTime){
    return waitQueue->modify(roomId,fanSpeed,waitTime);
}
bool Scheduler::searchRoomWait(QString roomId){
    return waitQueue->exists(roomId);
}
bool Scheduler::addToWaitQueue(QString roomId, int fanSpeed, int waitTime){
    return waitQueue->add(roomId,fanSpeed,waitTime);
}
//service
bool Scheduler::changeServiceInServiceQueue(QString oldId, QString newId, int fanSpeed, int startTime){
    return serviceQueue->change(oldId,newId,fanSpeed,startTime);
}
bool Scheduler::modifyServiceQueue(QString roomId, int fanSpeed,int startTime){
    return serviceQueue->modify(roomId,fanSpeed,startTime);
}
bool Scheduler::searchRoomService(QString roomId){
    return serviceQueue->exists(roomId);
}
bool Scheduler::addToServiceQueue(QString roomId, int fanSpeed, int startTime){
    return serviceQueue->add(roomId,fanSpeed,startTime);
}
void Scheduler::checkTimeout(){
    QVector<QString> timeouts=waitQueue->checkTimeout(TIME_SLOT);
    for(QVector<QString>::iterator it=timeouts.begin();it!=timeouts.end();++it){
        QString id=*it;
        int speed=waitQueue->getSpeed(id);
        schedWait(id,speed);
    }
}
QString Scheduler::compareFanSpeedWait(int fanSpeed){
    return waitQueue->compare(fanSpeed);
}
QString Scheduler::compareFanSpeedService(int fanSpeed){
    return serviceQueue->compare(fanSpeed);
}
bool Scheduler::schedWait(QString roomId, int fanSpeed){
    QTime currentTime=QTime::currentTime();
    int timenow=currentTime.hour()*3600+currentTime.minute()*60+currentTime.second();
    /*先直接加入服务队列，如果不行则需要进行调度*/
    if(!addToServiceQueue(roomId,fanSpeed,timenow)){
        QString res=compareFanSpeedService(fanSpeed);
        if(res=="none"){
            modifyWaitQueue(roomId,fanSpeed,TIME_SLICE);
        }else{
            int newSpeed=serviceQueue->getSpeed(res);
            changeServiceInWaitQueue(roomId,res,newSpeed,TIME_SLICE);
            changeServiceInServiceQueue(res,roomId,fanSpeed,timenow);
            changeFanSpeedOfAc(res,0);
            changeFanSpeedOfAc(roomId,fanSpeed);
        }
        return false;
    }
    return true;
}

bool Scheduler::schedNew(QString roomId, int fanSpeed){
    QTime currentTime=QTime::currentTime();
    int timenow=currentTime.hour()*3600+currentTime.minute()*60+currentTime.second();
    /*先直接加入服务队列，如果不行则需要进行调度*/
    if(!addToServiceQueue(roomId,fanSpeed,timenow)){
        QString res=compareFanSpeedService(fanSpeed);
        if(res=="none"){
            /*暂时不服务*/
            addToWaitQueue(roomId,fanSpeed,TIME_SLICE);
        }else{
            /*服务*/
            int newSpeed=serviceQueue->getSpeed(res);
            addToWaitQueue(res,newSpeed,TIME_SLICE);
            changeServiceInServiceQueue(res,roomId,fanSpeed,timenow);
            changeFanSpeedOfAc(res,0);
        }
        return false;
    }
    return true;
}
bool Scheduler::schedChange(QString roomId, int fanSpeed){
    QTime currentTime=QTime::currentTime();
    int timenow=currentTime.hour()*3600+currentTime.minute()*60+currentTime.second();
    if(searchRoomService(roomId)){
        QString res=compareFanSpeedWait(fanSpeed);
        if(res!="none"){
            int newSpeed=waitQueue->getSpeed(res);
            changeServiceInWaitQueue(res,roomId,fanSpeed,TIME_SLICE);
            changeServiceInServiceQueue(roomId,res,newSpeed,timenow);
            changeFanSpeedOfAc(roomId,0);
            changeFanSpeedOfAc(res,newSpeed);
        }else{
            modifyServiceQueue(roomId,fanSpeed,timenow);
            changeFanSpeedOfAc(roomId,fanSpeed);
        }
    }else if(searchRoomWait(roomId)){
        QString res=compareFanSpeedService(fanSpeed);
        if(res=="none"){
            modifyWaitQueue(roomId,fanSpeed,TIME_SLICE);
        }else{
            int newSpeed=serviceQueue->getSpeed(res);
            changeServiceInWaitQueue(roomId,res,newSpeed,TIME_SLICE);
            changeServiceInServiceQueue(res,roomId,fanSpeed,timenow);
            changeFanSpeedOfAc(res,0);
            changeFanSpeedOfAc(roomId,fanSpeed);
        }
    }else{
        return false;
    }
    return true;
}
/*向空调对象发送的指令*/
bool Scheduler::powerOn(QString roomId, double currentTemp){
    if(schedNew(roomId,defaultFanSpeed)){
        AirCondition *air=new AirCondition();
        /*调用空调的开机方法*/
        airConditioners.append(air);
    }
}
bool Scheduler::changeFanSpeed(QString roomId, int fanSpeed){
    if(!schedNew(roomId,fanSpeed)){
        printf("RoomID does not exists\n");
        return false;
    }
    return true;
}
bool Scheduler::powerOff(QString roomId){
    QTime currentTime=QTime::currentTime();
    int timenow=currentTime.hour()*3600+currentTime.minute()*60+currentTime.second();
    if(searchRoomService(roomId)){
        serviceQueue->remove(roomId);
        QString res=compareFanSpeedWait(-1);
        if(res!="none"){
            int newSpeed=waitQueue->getSpeed(res);
            waitQueue->remove(res);
            addToServiceQueue(res,newSpeed,timenow);
            changeFanSpeedOfAc(res,newSpeed);
        }
    }else if(searchRoomWait(roomId)){
        waitQueue->remove(roomId);
    }else{
        return false;
    }
    /*删除列表中的空调对象*/
    for(QList<AirCondition*>::iterator it=airConditioners.begin();it!=airConditioners.end();it++){
        AirCondition* air=*it;
        if(air->roomId==roomId){
            delete air;
            airConditioners.erase(it);
            return true;
        }
    }
    return false;
}
