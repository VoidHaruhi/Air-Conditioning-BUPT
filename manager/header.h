/**
* @projectName   SEMpro2
* @brief         所有的宏定义，全部的外部依赖头文件引入
* @author        Oliver
* @date          2021-06-30
*/
#ifndef HEADER_H
#define HEADER_H
#include <QWidget>
#include<QLineEdit>
#include<QStringList>
#include<QDialog>
#include<random>
#include<QRandomGenerator>
#include<QChar>
#include<QDebug>
#include<QHeaderView>
#include<QRegExp>
#include<QString>
#include<QSize>
#include<QLineEdit>
#include<string>
#include<QJsonDocument>
#include<QTime>
#include<QKeyEvent>
#include<QTimer>
#include<QJsonArray>
#include<QWebSocket>
#include<QTableWidget>
#include<QKeyEvent>
#include<QJsonObject>
#include<QMessageBox>
#include<iostream>
#include<QTableWidgetItem>
using namespace std;
#include"md5.h"
#define REFID "refId"
#define HANDLER "handler"
#define DATA "data"
#define ADMINID "adminId"
#define MESSAGE  "msg"
#define TOKEN "token"
#define ROLE "role"
#define ROOMID "roomId"
#define WIND "wind"
#define NOWTEMP "nowTmp"
#define ISIDLE "idle"
#define TARTEMP "tmp"
#define POWER "power"
#define PASSWORD "password"
#define TEMPHLIMIT "tempHighLimit"
#define TEMPLLIMIT "tempLowLimit"
#define DEFAULTTARTEMP "defaultTargetTemp"
#define FEERATEH "feeRateH"
#define FEERATEM "feeRateM"
#define MODE "mode"
#define FEERATEL "feeRateL"
//#define SRVERADDR ""
//#define SRVERADDR "10.128.219.19"
#define SRVERADDR "127.0.0.1"
#define SRVERPORT "12345"
#define LEN_ROOMINFO 6
#define LEN_ROOMLIST 2
#define LEN_DETAILCOST 60
#define LEN_SIMCOST 4
QString generate_refId();
void setAddr();

extern QStringList refIdlsit;


#endif // HEADER_H
