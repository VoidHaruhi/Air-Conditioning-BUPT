/**
 * @title   main.cpp
 * @module  主函数
 * @author  刘宇航
 * @interface   主函数声明一个dispatcher对象，即可默认启动中央空调
 * @version  1.0    创建主函数
 */
#include <QCoreApplication>
#include "dispatcher.h"
#include "database.h"
#include "aircondition.h"
#include <windows.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Dispatcher d;
    return a.exec();
}
