#define _CRT_SECURE_NO_WARINGS
#include <QApplication>
#include <QDebug>
#include "mainwindow.h"
#include "WebSocketServer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    qDebug()<<WebSocketServer::generateMD5("1");
    return a.exec();
}
