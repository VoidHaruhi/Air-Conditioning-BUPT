#ifndef MNG_CTRL_H
#define MNG_CTRL_H

#include <QList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QDebug>
#include "database.h"
#include "aircondition.h"

class Report
{
public:
    QString roomId;
    double totalFee;
    int date_in;
    int date_out;
};

class ManagerController
{
public:
    ManagerController();
    Database *db;
    QList<Report> report;
    QJsonObject queryReport(QJsonObject msg);
};


#endif // MNG_CTRL_H
