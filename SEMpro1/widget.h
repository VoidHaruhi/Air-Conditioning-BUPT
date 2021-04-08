#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include"header.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE
enum super_type{
    manager = 1,
    admin = 2

};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QString token_local,QWidget *parent = nullptr,int type = 1);
    ~Widget();
    //发包
    void getAllroom();
    void openRoom(QString roomId,QString temp);
    void seeRoomInfo(QString roomId);
    void controlRoom(QString roomId,QString temp,QString power,QString wind);
    void getCost(QString roomId="all",bool detail=false,bool detailFIle=false);
    void sendMSG(QString roomId,QString msg);

    void recvMsg(const QString& msg);
    void connectSrv();
    void Iniconnect();
    void IniCtrlroom();
    void IniOpenroom();
    //接受处理特定的包
    void dealRoomlist(QJsonObject json);
    void dealFeeList(QJsonObject json);
    void dealError(QJsonObject json);
    void dealConfirm(QJsonObject json);
    void dealRoomInfo(QJsonObject json);
    void IniWidget();
private slots:
    void on_allroomInfo_btn_clicked();

    void on_roomInfo_btn_clicked();

    void on_checkBox_2_stateChanged(int arg1);

    void on_checkBox_stateChanged(int arg1);

    void on_checkBox_3_stateChanged(int arg1);

    void on_ctrlroom_btnbox_accepted();

    void on_ctrlroom_btnbox_rejected();

    void on_openroom_btnbox_accepted();

    void on_openroom_btnbox_rejected();

    void on_Cost_clicked();

private:
    QStringList sl ={
        "/server/retRoomList",
        "/server/retFeeList",
        "/server/retSimpleCost",
        "/server/roomInfo",
        "/server/comfirm",
        "/server/error"
    };
    QWebSocket* client;
    Ui::Widget *ui;
    QString token="101";
};
#endif // WIDGET_H
