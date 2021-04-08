#include "widget.h"
#include"Login.h"
#include <QApplication>
QStringList refIdlsit={};
int main(int argc, char *argv[])
{


    QApplication a(argc, argv);
//    Login w;


//    w.show();
    Widget w("101");
    w.show();
    return a.exec();



}
