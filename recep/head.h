/**
 * @title head.h
 * @author 唐方柳
 */
#ifndef HEAD_H
#define HEAD_H
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
#include<QTimer>
#include<QJsonArray>
#include<QWebSocket>
#include<QTableWidget>
#include<QKeyEvent>
#include<QJsonObject>
#include<QMessageBox>
#include<iostream>
#include<QTableWidgetItem>
#include <QPainter>
#include <QIcon>
#include <QDateTime>
#include <qinputdialog.h>
#include <QDebug>
#include "md5.h"
#define SERVER_URL "ws://127.0.0.1:12345"
using namespace std;

extern QStringList refIdlist;
extern QString refId;
extern QString token;
extern QWebSocket *reception;

/**
 * @brief generate_refId 生成数据包识别id
 * @return 返回数据包识别id
 * @author 唐方柳
 */
QString generate_refId();
#endif // HEAD_H
