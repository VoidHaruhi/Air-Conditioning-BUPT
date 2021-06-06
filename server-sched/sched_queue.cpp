#include "sched_queue.h"

QueueType::QueueType(QString id,int speed,int wt,int st){
    roomId=id;
    fanSpeed=speed;
    waitTime=wt;
    startTime=st;
}
QueueType::QueueType(){
    roomId=-1;
}


BasicQueue::BasicQueue(){
    roomList.clear();
}
void BasicQueue::insert(QueueType q){
    this->roomList.append(q);
}
QueueType BasicQueue::remove(QString id){
    QVector<QueueType>::iterator it;
    QueueType res;
    for(it=roomList.begin();it!=roomList.end();++it){
        if(it->roomId==id){
            res=*it;
            roomList.erase(it);
            break;
        }
    }
    return res;
}
void BasicQueue::update(QueueType q){
    QVector<QueueType>::iterator it;
    for(it=roomList.begin();it!=roomList.end();++it){
        if(it->roomId==q.roomId){
            it->fanSpeed=q.fanSpeed;
            it->waitTime=q.waitTime;
            it->startTime=q.startTime;
            return;
        }
    }
}

int BasicQueue::getSpeed(QString roomId){
    QVector<QueueType>::iterator it;
    for(it=roomList.begin();it!=roomList.end();++it){
        if(it->roomId==roomId){
            return it->fanSpeed;
        }
    }
    return -1;
}
QVector<QString> WaitQueue::checkTimeout(int timepassed){
    QVector<QString> res;
    QVector<QueueType>::iterator it;
    for(it=roomList.begin();it!=roomList.end();++it){
        it->waitTime-=timepassed;
        if(it->waitTime<=0){
            res.append(it->roomId);
        }
    }
    return res;
}

bool WaitQueue::change(QString oldId, QString newId, int fanSpeed, int waitTime){
    QVector<QString> res;
    QVector<QueueType>::iterator it;
    for(it=roomList.begin();it!=roomList.end();++it){
        if(it->roomId==oldId){
            it->fanSpeed=fanSpeed;
            it->waitTime=waitTime;
            return true;
        }
    }
    return false;
}

bool WaitQueue::modify(QString roomId, int fanSpeed, int waitTime){
    QVector<QString> res;
    QVector<QueueType>::iterator it;
    for(it=roomList.begin();it!=roomList.end();++it){
        if(it->roomId==roomId){
            it->fanSpeed=fanSpeed;
            if(waitTime>=0){
                it->waitTime=waitTime;
            }
            return true;
        }
    }
    return false;
}

QString WaitQueue::compare(int fanSpeed){
    QString maxRoom="none";
    int maxSpeed=fanSpeed,maxTime;
    QVector<QueueType>::iterator it;
    for(it=roomList.begin();it!=roomList.end();++it){
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
    return maxRoom;
}

bool WaitQueue::add(QString roomId, int fanSpeed, int waitTime){
    QueueType tmp(roomId,fanSpeed,waitTime,-1);
    roomList.append(tmp);
    return true;
}

bool ServiceQueue::change(QString oldId, QString newId, int fanSpeed, int startTime){
    QVector<QString> res;
    QVector<QueueType>::iterator it;
    for(it=roomList.begin();it!=roomList.end();++it){
        if(it->roomId==oldId){
            it->roomId=newId;
            it->fanSpeed=fanSpeed;
            it->startTime=startTime;
            return true;
        }
    }
    return false;
}
bool ServiceQueue::modify(QString roomId, int fanSpeed, int startTime){
    QVector<QString> res;
    QVector<QueueType>::iterator it;
    for(it=roomList.begin();it!=roomList.end();++it){
        if(it->roomId==roomId){
            it->fanSpeed=fanSpeed;
            it->startTime=startTime;
            return true;
        }
    }
    return false;
}
bool BasicQueue::exists(QString roomId){
    QVector<QString> res;
    QVector<QueueType>::iterator it;
    for(it=roomList.begin();it!=roomList.end();++it){
        if(it->roomId==roomId)return true;
    }
    return false;
}
bool ServiceQueue::add(QString roomId, int fanSpeed, int startTime){
    QueueType tmp(roomId,fanSpeed,-1,startTime);
    if(roomList.length()>=MAX_SERVICE){
        return false;
    }
    roomList.append(tmp);
    return true;
}
QString ServiceQueue::compare(int fanSpeed){
    QString minRoom="none";
    int minSpeed=fanSpeed,minTime=3600*24+1;
    QVector<QueueType>::iterator it;
    for(it=roomList.begin();it!=roomList.end();++it){
        if(it->fanSpeed<minSpeed){
            minRoom=it->roomId;
            minSpeed=it->fanSpeed;
            minTime=it->startTime;
        }else{
            if(it->fanSpeed==minSpeed&&it->startTime<minTime){//替换最小的
                minRoom=it->roomId;
                minSpeed=it->fanSpeed;
                minTime=it->startTime;
            }
        }
    }
    return minRoom;
}
