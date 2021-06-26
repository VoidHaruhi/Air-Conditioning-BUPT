#include "scheduler.h"
#include "header.h"
#include<QDebug>

Scheduler::Scheduler(QWebSocketServer *sv,std::map<QString,QWebSocket*> *sp){
    defaultMode = 0;
    defaultFanSpeed = 0;
    defaultTempLimit[0] = 16;
    defaultTempLimit[1] = 27;
    defaultTargetTemp = 25;
    sockMap=sp;
    server=sv;
    serviceQueue=new ServiceQueue();
    waitQueue=new WaitQueue();
    airConditioners.clear();
    timer=new QTimer();
    timerWork=new QTimer();
    timer->setInterval(1000*TIME_SLOT);
    connect(timer,&QTimer::timeout,this,&Scheduler::checkTimeout);
    timerWork->setInterval(6000);  /*假设每分钟回温0.5摄氏度,一个时间片是6秒*/
    connect(timerWork,&QTimer::timeout,this,&Scheduler::rewarm);
    timer->start();
    timerWork->start();
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
    timer->stop();
    QVector<QString> timeouts=waitQueue->checkTimeout(TIME_SLOT);
    for(int i=0;i<timeouts.length();i++){
        QString id=timeouts[i];
        int speed=waitQueue->getSpeed(id);
        schedWait(id,speed);
    }
    timer->start();
}
QString Scheduler::compareFanSpeedWait(int fanSpeed){
    return waitQueue->compare(fanSpeed);
}
QString Scheduler::compareFanSpeedService(int fanSpeed){
    QString res=serviceQueue->compare(fanSpeed);
    qDebug()<<"Compare in Service Queue: speed"<<fanSpeed<<" result:"<<res;
    return res;
}
bool Scheduler::schedWait(QString roomId, int fanSpeed){
    QTime currentTime=QTime::currentTime();
    int timenow=currentTime.hour()*3600+currentTime.minute()*60+currentTime.second();
    AirCondition *ac;
    /*先直接加入服务队列，如果不行则需要进行调度*/
    if(!addToServiceQueue(roomId,fanSpeed,timenow)){
        QStringList res=compareFanSpeedService(fanSpeed).split('/');
        if(res[0]=="small"){
            modifyWaitQueue(roomId,fanSpeed,TIME_MAX);
        }else if(res[0]=="equal"){
            modifyWaitQueue(roomId,fanSpeed,TIME_SLICE);
        }else{
            QString id=res[1];
            int newSpeed=serviceQueue->getSpeed(id);
            changeServiceInWaitQueue(roomId,id,newSpeed,TIME_SLICE);
            changeServiceInServiceQueue(id,roomId,fanSpeed,timenow);
            changeFanSpeedOfAc(id,0);
            changeFanSpeedOfAc(roomId,fanSpeed);
            ac=getAc(roomId);
            if(ac){
                ac->onSched(1);
            }
            ac=getAc(id);
            if(ac){
                ac->onSched(1);
            }
        }
        return false;
    }
    ac=getAc(roomId);
    ac->onSched(1);
    return true;
}
bool Scheduler::schedNew(QString roomId, int fanSpeed){
    QTime currentTime=QTime::currentTime();
    int timenow=currentTime.hour()*3600+currentTime.minute()*60+currentTime.second();
    AirCondition *ac;
    /*先直接加入服务队列，如果不行则需要进行调度*/
    if(!addToServiceQueue(roomId,fanSpeed,timenow)){
        QStringList res=compareFanSpeedService(fanSpeed).split('/');
        if(res[0]=="small"){
            /*暂时不服务*/
            addToWaitQueue(roomId,fanSpeed,TIME_MAX);
        }else if(res[0]=="equal"){
            addToWaitQueue(roomId,fanSpeed,TIME_SLICE);
        }else{
            /*服务*/
            QString id=res[1];
            int newSpeed=serviceQueue->getSpeed(id);
            addToWaitQueue(id,newSpeed,TIME_SLICE);
            ac=getAc(id);
            ac->onSched(1);
            changeServiceInServiceQueue(id,roomId,fanSpeed,timenow);
            changeFanSpeedOfAc(id,0);
            changeFanSpeedOfAc(roomId,fanSpeed);
        }
        return false;
    }
    return true;
}
bool Scheduler::schedChange(QString roomId, int fanSpeed){
    QTime currentTime=QTime::currentTime();
    int timenow=currentTime.hour()*3600+currentTime.minute()*60+currentTime.second();
    AirCondition *ac;
    if(searchRoomService(roomId)){/*要调风的空调在服务队列*/
        /*先把*/
        modifyServiceQueue(roomId,fanSpeed,timenow);
        changeFanSpeedOfAc(roomId,fanSpeed);
    }else if(searchRoomWait(roomId)){
        QString res=compareFanSpeedService(fanSpeed);
        waitQueue->remove(roomId);
        schedNew(roomId,fanSpeed);
    }else{
        return false;
    }
    return true;
}

bool Scheduler::schedDel(){
    QTime currentTime=QTime::currentTime();
    int timenow=currentTime.hour()*3600+currentTime.minute()*60+currentTime.second();
    QString res=compareFanSpeedWait(-1);
    AirCondition *ac;
    if(res!="none"){
        int newSpeed=waitQueue->getSpeed(res);
        waitQueue->remove(res);
        addToServiceQueue(res,newSpeed,timenow);
        changeFanSpeedOfAc(res,newSpeed);
        ac=getAc(res);
        if(ac){
            ac->onSched(1);
        }
        return true;
    }
    return false;
}

bool Scheduler::powerOn(QString roomId, double currentTemp)
{
    AirCondition *air=new AirCondition();
    air->initial(roomId, currentTemp);
    airConditioners.append(air);
    return true;
}

bool Scheduler::powerOff(QString roomId)
{
    QTime currentTime=QTime::currentTime();
    int timenow=currentTime.hour()*3600+currentTime.minute()*60+currentTime.second();
    /*从队列中删除*/
    if(searchRoomService(roomId)){
        serviceQueue->remove(roomId);

    }else if(searchRoomWait(roomId)){
        waitQueue->remove(roomId);
    }else{
        return false;
    }
    //airConditioners.removeOne(air);
    AirCondition *ac=getAc(roomId);
    if(ac!=NULL){
        airConditioners.removeOne(ac);
    }else{
        return false;
    }
    //-----------------

    for(int i=0;i<airConditioners.length();i++){
        AirCondition* air=airConditioners[i];
        if(air->roomId==roomId){
            delete air;
            airConditioners.removeAt(i);
            return true;
        }
    }
    return false;

}

bool Scheduler::startAc(QString roomId, int mode, double targetTemp, double *rateFeeTable, double currentFee, double totalFee)
{
    AirCondition *ac=getAc(roomId);
    if(ac!=NULL){
        ac->start(mode, targetTemp, rateFeeTable);
    }else{
        return false;
    }
    return true;
}

bool Scheduler::changeTargetTemp(QString roomId, double targetTemp)
{
    AirCondition *ac=getAc(roomId);
    if(ac!=NULL){
        ac->changeTargetTemp(targetTemp);
        return true;
    }else{
        return false;
    }
}

bool Scheduler::changeFanSpeed(QString roomId, int fanSpeed)
{
    if(!schedNew(roomId,fanSpeed)){
        printf("RoomID does not exists\n");
        return false;
    }
    return false;
}

bool Scheduler::requestTempUp(QString roomId)
{
     for(int i=0; i<airConditioners.size(); i++){
         AirCondition* ac = airConditioners[i];
         if(ac->roomId == roomId && ac->mode == 0){  /* 制冷模式 */
             if(ac->currentTemp <= (ac->initialTemp-TEMP_CHANGE)){  /*定时器为6s,每次回温0.05度*/

                 ac->currentTemp = ac->initialTemp;
             }
             else{
                 ac->currentTemp += TEMP_CHANGE;
             }
         }
         else if(ac->roomId == roomId && ac->mode == 1){ /* 制热模式 */
             if(ac->currentTemp >= (ac->initialTemp-TEMP_CHANGE)){
                 ac->currentTemp = ac->initialTemp;
             }
             else{
                 ac->currentTemp -= TEMP_CHANGE;
             }
         }
     }

     return true;
}

bool Scheduler::requestRelease(QString roomId)
{
    /*修改：需要将该空调加入到服务队列 */
    schedNew(roomId,defaultFanSpeed);
    return true;
}

void Scheduler::changeFanSpeedOfAc(QString roomId, int fanSpeed)
{
    AirCondition *ac=getAc(roomId);
    if(ac!=NULL){
        ac->changeFanSpeed(fanSpeed);
        changeAc((*sockMap)[roomId],1,ac->currentTemp,fanSpeed);
    }
}

void Scheduler::rewarm()
{
    timerWork->stop();
    /* 遍历空调列表，判断需要回温的空调 */
    for(int i=0; i<airConditioners.size(); i++){
        AirCondition* ac = airConditioners[i];
        /* 空调处于关闭状态 */
        if(ac->isStart == false){
            /* 当前温度不等于默认温度，需要回温 */
            if(ac->currentTemp != ac->initialTemp){
                requestTempUp(ac->roomId);
            }
            /* 当前温度与目标温度超过1度，需要重新启动空调 */
            if(abs(ac->currentTemp - ac->initialTemp) >= 1){
                requestRelease(ac->roomId);
            }
        }
    }

    /*修改：遍历服务队列，修改温度 */
    for(int i=0;i<serviceQueue->roomList.length();i++){
        QString id=serviceQueue->roomList[i].roomId;
        AirCondition* ac;
        for(int i=0;i<airConditioners.length();i++){
            ac=airConditioners[i];
            if(ac->roomId==id){
                break;
            }
        }

        /* 计算温度变化量 */
        double change = ac->fanSpeed * TEMP_CHANGE;
        if(change <= abs(ac->currentTemp - ac->targetTemp)){
            ac->currentTemp = ac->targetTemp;
            /*修改：结束工作 */
            ac->end();
            serviceQueue->remove(ac->roomId);
            ac->onSched(1);
            schedDel();
        }
        else{
            /* 制冷且处于服务队列 */
            if(ac->mode == 0){
                ac->currentTemp -= change;
            }
            /* 制热且处于服务队列 */
            else if(ac->mode == 1){
                ac->currentTemp += change;
            }
        }
    }

    timerWork->start();
}

void Scheduler::changeAc(QWebSocket *sock, int power, double tmp, int wind){
    QJsonObject ret;
    ret.insert(REFID,"1490185183");
    ret.insert(HANDLER,"/server/change");
    QJsonObject data;
    data.insert("power",power);
    data.insert("tmp",tmp);
    data.insert("wind",wind);
    ret.insert(DATA,data);
    SEND_MESSAGE(sock,ret);
}

QJsonObject Scheduler::managerAnalyse(QJsonObject msg)
{
    QJsonObject answer;
    answer.insert(REFID, msg[REFID]);
    QStringList request = msg[HANDLER].toString().split('/');

    if(request[2] == "powerOn"){
        QJsonObject data = answer[DATA].toObject();
        if(data["mode"] == "heat"){
            defaultMode = 1;
        }
        else if(data["mode"] == "cold"){
            defaultMode = 0;
        }
        defaultTempLimit[0] = data["tempLowLimit"].toDouble();
        defaultTempLimit[1] = data["tempHighLimit"].toDouble();
        defaultTargetTemp = data["defaultTargetTemp"].toDouble();
        rateFeeTable[0] = 0;
        rateFeeTable[1] = data["feeRateL"].toDouble();
        rateFeeTable[2] = data["feeRateM"].toDouble();
        rateFeeTable[3] = data["feeRateH"].toDouble();
        /* state 没涉及到 */
        answer.insert(HANDLER,"/server/confirm");
    }
    else if(request[2] == "changeFee"){
        QJsonObject data = answer[DATA].toObject();
        rateFeeTable[0] = 0;
        rateFeeTable[1] = data["feeRateL"].toDouble();
        rateFeeTable[2] = data["feeRateM"].toDouble();
        rateFeeTable[3] = data["feeRateH"].toDouble();
        /* state 没涉及到 */
        answer.insert(HANDLER,"/server/confirm");
    }else if(request[2] == "queryRoomState"){
        QJsonObject data = answer[DATA].toObject();
        AirCondition *ac;
        QJsonArray retList;
        for(int i=0; i<airConditioners.size(); i++){
            ac = airConditioners[i];
            QJsonObject item;
            item.insert("roomId", ac->roomId);
            QString isOn = "yes";
            if(ac->isStart==false)
                isOn = "no";
            item.insert("isPowerOn",isOn);
            item.insert("currentTemp", ac->currentTemp);
            item.insert("targetTemp", ac->targetTemp);
            item.insert("fanSpeed", ac->fanSpeed);
            item.insert("fee", ac->totalFee);
            item.insert("state", "wait");
            retList.append(item);
        }
        answer.insert(DATA, retList);
        answer.insert(HANDLER,"/server/roomState");
    }else{
        answer.insert(HANDLER,"/server/error");
    }
    return answer;
}

QJsonObject Scheduler::acAnalyse(QJsonObject msg)
{
    QJsonObject answer;
    answer.insert(REFID, msg[REFID]);
    QStringList request = msg[HANDLER].toString().split('/');

    if(request[2] == "init"){
        QJsonObject data = msg[DATA].toObject();
        QString roomId = data["roomId"].toString();
        double initTemp = data["initTmp"].toDouble();
        /* 新建一个空调对象 */
        powerOn(roomId, initTemp);
        answer.insert(HANDLER,"/server/confirm");
    }
    else if(request[2] == "controlRoom"){
        QJsonObject data = msg[DATA].toObject();
        QString roomId = data["roomId"].toString();
        int power = data["power"].toInt();
        double tmp = data["power"].toDouble();
        int wind = data["wind"].toInt();

        AirCondition *ac;
        for(int i=0; i<airConditioners.size(); i++){
            ac = airConditioners[i];
            if(ac->roomId == roomId)
                break;
        }
        if(power != -1){
            /* 从关机到开机 */
            if(power == 1 && ac->isStart == false){
                startAc(roomId,defaultMode,defaultTargetTemp,rateFeeTable,0,0);
                changeAc(sockMap->at(roomId),1,defaultTargetTemp,defaultFanSpeed);
            }
            /* 从开机到关机 */
            else if(power == 0 && ac->isStart == true){
                serviceQueue->remove(ac->roomId);
                ac->onSched(1);
                schedDel();
                ac->end();
                changeAc(sockMap->at(roomId),0,defaultTargetTemp,0);
            }
        }
        if(tmp != -1){
            changeTargetTemp(roomId,tmp);
            changeAc(sockMap->at(roomId),1,tmp,ac->fanSpeed);
        }
        if(wind != -1){
            changeFanSpeed(roomId,wind);
            changeAc(sockMap->at(roomId),1,ac->targetTemp,wind);
        }

        answer.insert(HANDLER,"/server/confirm");
    }
    else if(request[2] == "requestState"){
        QJsonObject data = msg[DATA].toObject();
        QString roomId = data["roomId"].toString();

        AirCondition *ac=getAc(roomId);
        if(!ac){
            answer.insert(HANDLER,"/server/error");
            return answer;
        }
        QJsonObject retData;
        retData.insert("currentFee",ac->currentFee);
        retData.insert("totalFee",ac->totalFee);
        int isOn = 1;
        if(ac->isStart==false)
            isOn = 0;
        retData.insert("isOn",isOn);
        retData.insert("currentTemp",ac->currentTemp);
        retData.insert("targetTemp",ac->targetTemp);
        retData.insert("fanSpeed",ac->fanSpeed);
        retData.insert("rateFee",ac->rateFee);
        answer.insert(DATA,retData);
        answer.insert(HANDLER,"/server/confirm");
    }
    else if(request[2] == "openRoom"){
        answer.insert(HANDLER,msg[HANDLER]);
        QJsonObject data = msg[DATA].toObject();
        data.insert("tempLow",defaultTempLimit[0]);
        data.insert("tempHigh",defaultTempLimit[1]);
        data.insert("mode",defaultMode);
        data.insert("defaultTmp",defaultTargetTemp);
        data.insert("defaultWind",defaultFanSpeed);
        answer.insert(DATA,data);
    }
    else{
        answer.insert(HANDLER,"/server/error");
    }
    return answer;
}
AirCondition *Scheduler::getAc(QString roomId){
    AirCondition *ac=NULL;
    for(int i=0; i<airConditioners.size(); i++){
        ac = airConditioners[i];
        if(ac->roomId == roomId){
            ac=airConditioners[i];
            break;
        }
    }
    if(!ac){
        qDebug()<<"Room not found:"<<roomId;
    }
    return ac;
}
