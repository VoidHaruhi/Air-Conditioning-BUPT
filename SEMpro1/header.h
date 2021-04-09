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
#include<QRegExp>
#include<QString>
#include<string>
#include<QJsonDocument>
#include<QTime>
#include<QKeyEvent>
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
#define DEFAULTMP "defaultTmp"
#define ROOMID "roomId"
#define WIND "wind"
#define NOWTEMP "nowTmp"
#define ISIDLE "idle"
#define TARTEMP "tmp"
#define POWER "power"
#define PASSWORD "password"
#define SRVERADDR "127.0.0.1"
#define SRVERPORT "12345"
#define LEN_ROOMINFO 5
#define LEN_ROOMLIST 2
QString generate_refId();


extern QStringList refIdlsit;


#endif // HEADER_H