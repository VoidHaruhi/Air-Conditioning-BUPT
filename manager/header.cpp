#include"header.h"


/**
* @projectName   SEMpro2
* @brief         外部函数生成refID
* @author        Oliver
* @date          2021-06-30
*/

QString generate_refId()
{
    QTime *time = new QTime();

    QString ref;
    random_device rd;
    ref = time->currentTime().toString("hhmmsszzz");
    int i  = QRandomGenerator::global()->bounded(100);
    if(i<=9)  ref = ref+QString::number(0)+QString::number(i);
    else ref = ref+QString::number(i);
    qDebug()<<ref<<ref.toDouble();
    refIdlsit.append(ref);
    return ref;
}
