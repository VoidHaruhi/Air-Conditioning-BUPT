#ifndef RCP_CTRL_H
#define RCP_CTRL_H

#include <QList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QDebug>
#include "aircondition.h"
#include "database.h"

class ReceptionController
{
public:
    ReceptionController();
    Database *db;
    QList<RecordDetail> rd;
    QJsonObject analyse(QJsonObject msg);
    double createInvoice(QString roomId);
    QList<RecordDetail> createRD(QString roomId);
};


#endif // RCP_CTRL_H
