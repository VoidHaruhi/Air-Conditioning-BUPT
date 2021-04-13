#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QWidget>
#include <QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>
namespace Ui {
class WebSocketClient;
}

class MsgUnit
{
public:
    long long refID;
    int temp; //温度
    int wind; //风速
};

//客户端
class WebSocketClient : public QWidget
{
    Q_OBJECT

public:
    //统一添加前缀P ，用来和其他
    float Ptmp;//用户请求温度
    float PnowTmp;//当前房间温度
    float PinitTmp;//初始房间温度
    float Pintval;//时间间隔
    int Pspeed;//用户设定风速
    int Pnowspeed;//当前风速
    int Ppower;//开关机
    int Pkind;//冷热风
    explicit WebSocketClient(QWidget *parent = nullptr);
    ~WebSocketClient();

private slots:
    void on_spinBox_valueChanged(int arg1);

    void on_opening_clicked();

    void on_opening_clicked(bool checked);

    void on_opening_stateChanged(int arg1);

    void on_spinBox_2_valueChanged(int arg1);

    //void on_spinBox_2_valueChanged(int arg1);

    void on_makecold_clicked(bool checked);

    void on_makehot_clicked(bool checked);

    void sendMsgPackage();

    void packageAnalyse(QJsonObject recvJson);

    void on_SendButton_clicked();

    void sendMsgPackage_timer();

    void sendACK(int kind ,long long refId); //kind表示要发送的类型 0为普通的ack ,1为client/open 2为close

    void buttoncd();

    void SimulateTmp(); //模拟温度用的函数
signals:
    void test0();

private:
    Ui::WebSocketClient *ui;

    QWebSocket *client;
};

#endif // WEBSOCKETCLIENT_H
