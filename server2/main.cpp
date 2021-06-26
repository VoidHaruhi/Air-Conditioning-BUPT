#include <QCoreApplication>
#include "dispatcher.h"
#include "database.h"
#include "aircondition.h"
#include <windows.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Dispatcher d;

//    Database db;


//    AirCondition ac;
//    ac.initial("304c",24);

//    ac.changeFanSpeed(1);
//    Sleep(2000);
//    ac.totalFee = 4;
//    ac.currentFee = 2;
//    ac.changeFanSpeed(2);


    return a.exec();
}
