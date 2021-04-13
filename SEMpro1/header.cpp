#include"header.h"


//void setAddr()
//{
//    QFile file(":src/address.txt");
//          if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//          {
//              qDebug()<<"Open file error!";
//              return;
//            }
//          while (!file.atEnd()) {
////              QByteArray line = file.readLine();
//              QString line = file.readLine();
//              QStringList list = line.split(" ");
//              if(list[1].at(list[1].size()-1)=="\n")
//              {
//                  list[1] = list[1];
//              }
//              qDebug()<<list[0]<<list[1];

//    }
//}

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
