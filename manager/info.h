#ifndef INFO_H
#define INFO_H

#include <QScrollArea>
#include"header.h"
namespace Ui {
class info;
}

class info : public QScrollArea
{
    Q_OBJECT

public:
    explicit info(QJsonObject json,QString type ="report",QWidget *parent = nullptr);
    void SaveCsv(QString type);
    ~info();

private:
    Ui::info *ui;
    QTableWidget* table=nullptr;
};

#endif // INFO_H
