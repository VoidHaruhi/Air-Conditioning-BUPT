#include "widget.h"
#include"Login.h"
//#include"header.h"
#include <QApplication>
QStringList refIdlsit={};
QString SERIP = "";
QString SERPORT = "";
int main(int argc, char *argv[])
{

    setAddr();
    QApplication a(argc, argv);
    Login w;


    w.show();
//    Widget w1("101");
//    w1.show();
    return a.exec();



}
