#include "sched_queue.h"
#include<QDebug>

QueueType::QueueType(QString id,int speed,int wt,int st){
    roomId=id;
    fanSpeed=speed;
    waitTime=wt;
    startTime=st;
}
QueueType::QueueType(){
    roomId=-1;
}
void QueueType::printQ(){
    qDebug()<<roomId<<fanSpeed<<waitTime<<startTime;
}

BasicQueue::BasicQueue(){
    roomList.clear();
}
void BasicQueue::insert(QueueType q){
    this->roomList.append(q);
}
QueueType BasicQueue::remove(QString id){
    QueueType res;
    for(int i=0;i<roomList.length();i++){
        QueueType *it=&roomList[i];
        if(it->roomId==id){
            res=*it;
            qDebug()<<"Queue remove";
            res.printQ();
            roomList.erase(it);
            break;
        }
    }
    return res;
}
void BasicQueue::update(QueueType q){
    for(int i=0;i<roomList.length();i++){
        QueueType *it=&roomList[i];
        if(it->roomId==q.roomId){
            it->fanSpeed=q.fanSpeed;
            it->waitTime=q.waitTime;
            it->startTime=q.startTime;
            qDebug()<<"WaitQueue Update";
            it->printQ();
            return;
        }
    }
}

int BasicQueue::getSpeed(QString roomId){
    for(int i=0;i<roomList.length();i++){
        QueueType *it=&roomList[i];
        if(it->roomId==roomId){
            return it->fanSpeed;
        }
    }
    return -1;
}
QVector<QString> WaitQueue::checkTimeout(int timepassed){
    QVector<QString> res;
    for(int i=0;i<roomList.length();i++){
        QueueType *it=&roomList[i];
        it->waitTime-=timepassed;
        if(it->waitTime<=0){
            res.append(it->roomId);
            qDebug()<<"WaitQueue Timeout:"<<it->roomId;
        }
    }
    return res;
}

bool WaitQueue::change(QString oldId, QString newId, int fanSpeed, int waitTime){
    QVector<QString> res;

    for(int i=0;i<roomList.length();i++){
        QueueType *it=&roomList[i];
        if(it->roomId==oldId){
            it->roomId=newId;
            it->fanSpeed=fanSpeed;
            it->waitTime=waitTime;
            qDebug()<<"WaitQueue change:";
            it->printQ();
            return true;
        }
    }
    return false;
}

bool WaitQueue::modify(QString roomId, int fanSpeed, int waitTime){
    QVector<QString> res;
    for(int i=0;i<roomList.length();i++){
        QueueType *it=&roomList[i];
        if(it->roomId==roomId){
            it->fanSpeed=fanSpeed;
            if(waitTime>=0){
                it->waitTime=waitTime;
            }
            qDebug()<<"WaitQueue modify";
            it->printQ();
            return true;
        }
    }
    return false;
}

QString WaitQueue::compare(int fanSpeed){
    QString maxRoom="none";
    int maxSpeed=fanSpeed,maxTime;
    for(int i=0;i<roomList.length();i++){
        QueueType *it=&roomList[i];
        if(it->fanSpeed>maxSpeed){
            maxRoom=it->roomId;
            maxSpeed=it->fanSpeed;
            maxTime=it->waitTime;
        }else{
            if(it->startTime<maxTime){//替换最小的
                maxRoom=it->roomId;
                maxSpeed=it->fanSpeed;
                maxTime=it->waitTime;
            }
        }
    }
    qDebug()<<"Wait Queue compare:"<<maxRoom;
    return maxRoom;
}

bool WaitQueue::add(QString roomId, int fanSpeed, int waitTime){
    QueueType tmp(roomId,fanSpeed,waitTime,-1);
    roomList.append(tmp);
    qDebug()<<"Wait Queue add";
    tmp.printQ();
    return true;
}

bool ServiceQueue::change(QString oldId, QString newId, int fanSpeed, int startTime){
    QVector<QString> res;
    for(int i=0;i<roomList.length();i++){
        QueueType *it=&roomList[i];
        if(it->roomId==oldId){
            it->roomId=newId;
            it->fanSpeed=fanSpeed;
            it->startTime=startTime;
            qDebug()<<"ServiceQueue change:";
            it->printQ();
            return true;
        }
    }
    return false;
}
bool ServiceQueue::modify(QString roomId, int fanSpeed, int startTime){
    QVector<QString> res;
    for(int i=0;i<roomList.length();i++){
        QueueType *it=&roomList[i];
        if(it->roomId==roomId){
            it->fanSpeed=fanSpeed;
            it->startTime=startTime;
            qDebug()<<"ServiceQueue modify";
            it->printQ();
            return true;
        }
    }
    return false;
}
bool BasicQueue::exists(QString roomId){
    QVector<QString> res;
    for(int i=0;i<roomList.length();i++){
        if(roomList[i].roomId==roomId)return true;
    }
    return false;
}
bool ServiceQueue::add(QString roomId, int fanSpeed, int startTime){
    QueueType tmp(roomId,fanSpeed,-1,startTime);
    if(roomList.length()>=MAX_SERVICE){
        return false;
    }
    roomList.append(tmp);
    qDebug()<<"Service Queue add";
    tmp.printQ();
    return true;
}
QString ServiceQueue::compare(int fanSpeed){
    QVector<QueueType> res_large,res_equal;
    for(int i=0;i<roomList.length();i++){
        if(roomList[i].fanSpeed<fanSpeed){
            res_large.append(roomList[i]);
        }else if(roomList[i].fanSpeed==fanSpeed){
            res_equal.append(roomList[i]);
        }
    }
    if(res_large.length()!=0){/*大于*/
        if(res_large.length()==1){/*只有一个低于*/
            return "big/"+res_large[0].roomId;
        }else{/*多个低于*/
            int tmpspeed=res_large[0].fanSpeed,minspeed=res_large[0].fanSpeed;
            QString minroom="none";
            for(int i=0;i<res_large.length();i++){
                if(res_large[i].fanSpeed<minspeed){
                    minspeed=res_large[i].fanSpeed;
                    minroom=res_large[i].roomId;
                }
            }
            if(tmpspeed==minspeed){/*多个低于且相等，用服务时长最大的*/
                int mintime=res_large[0].startTime;
                QString minroom="none";
                for(int i=0;i<res_large.length();i++){
                    if(res_large[i].startTime<mintime){
                        mintime=res_large[i].startTime;
                        minroom=res_large[i].roomId;
                    }
                }
                return "big/"+minroom;
            }else{/*不等，选风速最低的*/
                return "big/"+minroom;
            }
        }
    }else{
        if(res_equal.length()!=0){/*相等*/
            return "equal";
        }else{
            return "small";
        }
    }
}
