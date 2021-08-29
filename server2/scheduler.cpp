/**
 * @title   scheduler.cpp
 * @module  调度器控制器的实现
 * @author  于越
 * @interface   dispather调用该模块，进行解析，执行相应动作
 * @version v1.0 实现基本功能
 *          v2.0 在基础上实现了调度，进行debug
 */
#include "scheduler.h"
#include "header.h"
#include<QDebug>

Scheduler::Scheduler(QWebSocketServer *sv,std::map<QString,QWebSocket*> *sp){
    defaultMode = 0;
    defaultFanSpeed = 2;
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
    //timerWork->setInterval(1000*MY_MINUTE/10);  /*假设每分钟回温0.5摄氏度,一个时间片是6秒*/
    //connect(timerWork,&QTimer::timeout,this,&Scheduler::rewarm);
    timer->start();

    //timerWork->start();
}
/**
 * @brief Scheduler::setPara
 * @param mode
 * @param tempLimits
 * @param defaultTargetTemp
 * @param feeRateTable
 * @return
 */
bool Scheduler::setPara(int mode, double *tempLimits, double defaultTargetTemp, double *feeRateTable){
    this->defaultMode=mode;
    this->defaultTargetTemp=defaultTargetTemp;
    defaultFanSpeed=2;
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
    //qDebug()<<"Timeout";
    QVector<QString> timeouts=waitQueue->checkTimeout(TIME_SLOT);
    for(int i=0;i<timeouts.length();i++){
        QString id=timeouts[i];
        int speed=waitQueue->getSpeed(id);
        schedWait(id,speed);
    }
}
QString Scheduler::compareFanSpeedWait(int fanSpeed){
    return waitQueue->compare(fanSpeed);
}
QString Scheduler::compareFanSpeedService(int fanSpeed){
    QString res=serviceQueue->compare(fanSpeed);
    qDebug()<<"Compare in Service Queue: speed"<<fanSpeed<<" result:"<<res;
    return res;
}
/*等待队列有超时调用
 * 替换等待时间最长的*/
bool Scheduler::schedWait(QString roomId, int fanSpeed){
    QTime currentTime=QTime::currentTime();
    int timenow=currentTime.hour()*3600+currentTime.minute()*60+currentTime.second();
    AirCondition *ac;
    /*先直接加入服务队列，如果不行则需要进行替换*/
    if(!addToServiceQueue(roomId,fanSpeed,timenow)){
        QString res=serviceQueue->choose();
        /*res放入等待队列，roomId放入服务队列*/
        int newSpeed=serviceQueue->getSpeed(res);
        changeServiceInWaitQueue(roomId,res,newSpeed,TIME_SLICE);
        changeServiceInServiceQueue(res,roomId,fanSpeed,timenow);
        changeFanSpeedOfAc(res,0);
        changeFanSpeedOfAc(roomId,fanSpeed);
        ac=getAc(roomId);
        if(ac){
            ac->onSched(1);
        }
        ac=getAc(res);
        if(ac){
            ac->onSched(1);
        }
        return false;
    }
    changeFanSpeedOfAc(roomId,fanSpeed);
    qDebug()<<"SchedWait success";
    ac=getAc(roomId);
    ac->onSched(1);
    return true;
}
/*新的服务
 * 优先级调度，再时间片
*/
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
            //changeFanSpeedOfAc(roomId,0);
            return false;
        }else if(res[0]=="equal"){
            addToWaitQueue(roomId,fanSpeed,TIME_SLICE);
            //changeFanSpeedOfAc(roomId,0);
            return false;
        }else{
            /*服务并替换*/
            QString id=res[1];
            int newSpeed=serviceQueue->getSpeed(id);
            addToWaitQueue(id,newSpeed,TIME_SLICE);
            ac=getAc(id);
            ac->onSched(1);
            changeServiceInServiceQueue(id,roomId,fanSpeed,timenow);
            changeFanSpeedOfAc(id,0);
            //changeFanSpeedOfAc(roomId,fanSpeed);
            return true;
        }
    }
    //changeFanSpeedOfAc(roomId,fanSpeed);
    qDebug()<<"SchedNew success";
    return true;
}
/**
 * @brief Scheduler::schedChange
 * @param roomId
 * @param fanSpeed
 * @return 是否成功
 */
bool Scheduler::schedChange(QString roomId, int fanSpeed){
    QTime currentTime=QTime::currentTime();
    int timenow=currentTime.hour()*3600+currentTime.minute()*60+currentTime.second();
    if(fanSpeed==0){
        changeFanSpeedOfAc(roomId,fanSpeed);
        return true;
    }
    if(searchRoomService(roomId)){/*要调风的空调在服务队列*/
        /*直接调风即可*/
        modifyServiceQueue(roomId,fanSpeed,timenow);
        changeFanSpeedOfAc(roomId,fanSpeed);
    }else if(searchRoomWait(roomId)){
        waitQueue->remove(roomId);
        if(schedNew(roomId,fanSpeed)){
            changeFanSpeedOfAc(roomId,fanSpeed);
        }else{
            changeFanSpeedOfAc(roomId,0);
        }
    }else{
        if(schedNew(roomId,fanSpeed)){
            changeFanSpeedOfAc(roomId,fanSpeed);
        }else{
            changeFanSpeedOfAc(roomId,0);
        }
    }
    qDebug()<<"Sched Change Sucess";
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
    air->targetTemp=defaultTargetTemp;
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
        changeAc(sockMap->at(roomId),-1,targetTemp,-1);
        return true;
    }else{
        return false;
    }
}

bool Scheduler::changeFanSpeed(QString roomId, int fanSpeed)
{
    AirCondition *ac=getAc(roomId);
    if(ac==NULL){
        printf("RoomID does not exists\n");
        return false;
    }else if(ac->fanSpeed==fanSpeed){
        return false;
    }
    if(!schedChange(roomId,fanSpeed)){
        printf("RoomID does not exists\n");
        return false;
    }
    return false;
}

void Scheduler::changeFanSpeedOfAc(QString roomId, int fanSpeed)
{
    AirCondition *ac=getAc(roomId);
    if(ac!=NULL){
        ac->changeFanSpeed(fanSpeed);
        changeAc((*sockMap)[roomId],-1,-1,fanSpeed);
    }
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
        QJsonObject data = msg[DATA].toObject();
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
        /*查重*/
        AirCondition *ac=getAc(roomId);
        if(ac!=NULL){
            airConditioners.removeOne(ac);
        }
        /* 新建一个空调对象 */
        powerOn(roomId, initTemp);
        answer.insert(HANDLER,"/server/confirm");
    }
    else if(request[2] == "controlRoom"){
        QJsonObject data = msg[DATA].toObject();
        QString roomId = data["roomId"].toString();
        int power = data["power"].toInt();
        double tmp = data["tmp"].toDouble();
        int wind = data["wind"].toInt();
        qDebug()<<"Control:"<<roomId;
        AirCondition *ac;
        for(int i=0; i<airConditioners.size(); i++){
            ac = airConditioners[i];
            if(ac->roomId == roomId)
                break;
        }
        if(power != -1){
            /* 从关机到开机 */
            if(power == 1){
                startAc(roomId,defaultMode,tmp,rateFeeTable,0,0);
                if(schedNew(roomId,wind)){
                    ac->changeFanSpeed(wind);
                    changeAc(sockMap->at(roomId),1,tmp,wind);
                }else{
                    changeAc(sockMap->at(roomId),1,tmp,0);
                    ac->changeFanSpeed(0);
                }
            }
            /* 从开机到关机 */
            else if(power == 0){
                serviceQueue->remove(ac->roomId);
                ac->onSched(1);
                schedDel();
                ac->end();
                changeAc(sockMap->at(roomId),0,-1,0);

            }
            answer.insert(HANDLER,"/server/confirm");
            return answer;
        }
        if(tmp != -1){
            changeTargetTemp(roomId,tmp);
        }
        if(wind != -1){
            if(!ac->tempup)
                schedChange(roomId,wind);
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
        double cTemp = data["currentTmp"].toDouble();
        ac->setCurrentTemp(cTemp);
        retData.insert("currentFee",ac->currentFee);
        retData.insert("totalFee",ac->totalFee);
        answer.insert(DATA,retData);
        answer.insert(HANDLER,"/server/update");
    }
    else if(request[2] == "openRoom"){
        /*向空调发送*/
        answer.insert(HANDLER,msg[HANDLER]);
        QJsonObject data = msg[DATA].toObject();
        data.insert("tempLow",defaultTempLimit[0]);
        data.insert("tempHigh",defaultTempLimit[1]);
        data.insert("mode",defaultMode);
        data.insert("defaultTmp",defaultTargetTemp);
        data.insert("defaultWind",defaultFanSpeed);
        answer.insert(DATA,data);
    }else if(request[2]=="tempUp"){
        QJsonObject data = msg[DATA].toObject();
        QString roomId = data["roomId"].toString();
        AirCondition *ac=getAc(roomId);
        double currenttmp=data["currentTmp"].toDouble();
        ac->setCurrentTemp(currenttmp);
        ac->isStart=false;
        ac->tempup=true;
        /*不通知*/
        ac->changeFanSpeed(0);
        serviceQueue->remove(roomId);
        schedDel();
        answer.insert(HANDLER,"/server/confirm");
    }else if(request[2]=="tempRelease"){
        QJsonObject data = msg[DATA].toObject();
        QString roomId = data["roomId"].toString();
        int wind=data["wind"].toInt();
        double currenttmp=data["currentTmp"].toDouble();
        AirCondition *ac=getAc(roomId);
        ac->setCurrentTemp(currenttmp);
        ac->lastUpdateTemp=currenttmp;/*从停止回温时刻的温度开始算计费*/
        ac->isStart=true;
        ac->tempup=false;
        ac->targetTemp=data["tmp"].toDouble();
        if(schedNew(roomId,wind)){
            changeFanSpeedOfAc(roomId,wind);
        }else{
            changeFanSpeedOfAc(roomId,0);
        }
        answer.insert(HANDLER,"/server/confirm");
    }
    else{
        answer.insert(HANDLER,"/server/error");
    }
    return answer;
}
AirCondition *Scheduler::getAc(QString roomId){
    AirCondition *ans=NULL;
    AirCondition *ac=NULL;
    for(int i=0; i<airConditioners.size(); i++){
        ac=airConditioners[i];
        if(ac->roomId == roomId){
            ans=airConditioners[i];
            break;
        }
    }
    if(ans==NULL){
        qDebug()<<"Room not found:"<<roomId;
    }
    return ans;
}
