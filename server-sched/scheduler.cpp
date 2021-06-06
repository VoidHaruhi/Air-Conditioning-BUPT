#include "scheduler.h"

Scheduler::Scheduler(){
    serviceQueue=new ServiceQueue();
    waitQueue=new WaitQueue();
    timer=new QTimer();
    timer->setInterval(1000*TIME_SLOT);
    connect(timer,&QTimer::timeout,this,&Scheduler::checkTimeout);
}
bool Scheduler::setPara(int mode, double *tempLimits, double defaultTargetTemp, double *feeRateTable){
    this->defaultMode=mode;
    this->defaultTargetTemp=defaultTargetTemp;
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
bool Scheduler::searchRoom(QString roomId){
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
QString Scheduler::compareFanSpeed(int fanSpeed){
    return serviceQueue->compare(fanSpeed);
}

void Scheduler::schedWait(QString roomId, int fanSpeed){
    QTime currentTime=QTime::currentTime();
    int timenow=currentTime.hour()*3600+currentTime.minute()*60+currentTime.second();
    /*先直接加入服务队列，如果不行则需要进行调度*/
    if(!addToServiceQueue(roomId,fanSpeed,timenow)){
        QString res=compareFanSpeed(fanSpeed);
        if(res=="none"){
            modifyWaitQueue(roomId,fanSpeed,TIME_SLICE);
        }else{
            int newSpeed=serviceQueue->getSpeed(res);
            changeServiceInWaitQueue(roomId,res,newSpeed,TIME_SLICE);
            changeServiceInServiceQueue(res,roomId,fanSpeed,timenow);
            changeFanSpeed(res,0);
            changeFanSpeed(roomId,fanSpeed);
        }
    }
}
