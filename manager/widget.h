#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include"header.h"
#include"info.h"
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

    void changeFee(double feeRateH,double feeRateM,double feeRateL);//changeFee
    void powerOn(QString mode,int tempHighLimit,int tempLowLimit,int defaultTargetTemp,double feeRateH,double feeRateM,double feeRateL);//powerOn
    void openRoom(QString roomId);
    void queryreport(int date1,int date2,QString type);//queryreport
//    void queryRoomstate();//固定时间间隔给服务器发送queryRoomstate包
    void connectSrv();
    void Iniconnect();
    void queryRoomstate(QString roomId);


    void recvMsg(const QString& msg);
    void dealError(QJsonObject json);
    void dealConfirm(QJsonObject json);
    //接受处理特定的包
    void set_tartemp();
    void IniWidget();
private slots:
//    void so_connected();
     void queryRoomstate();//固定时间间隔给服务器发送queryRoomstate包

    void on_powerOn_btn_clicked();

    void on_report_btn_clicked();

    void on_start_supervise_clicked();

    void on_cold_cbox_stateChanged(int arg1);

    void on_heat_cbox_stateChanged(int arg1);

    void on_control_btn_clicked();

    void on_weekly_rbtn_clicked();

    void on_daily_rbtn_clicked();

    void on_openroom_btn_clicked();

private:
    QStringList sl ={
        "/server/roomState",
        "/server/report",
        "/server/confirm"
    };
    info *report=nullptr;
    info *roomstate=nullptr;
    QWebSocket* client;
    Ui::Widget *ui;
    bool isPower=false;
    bool isSupervise=false;
    QString token="101";
    QTimer *supervise_timer=nullptr;
};
#endif // WIDGET_H
