#include"header.h"



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
