/**
  @title widget.h
  @module 声明客户端中用于保存风速温度费用的类 声明交互的函数
  @author 李文淇
  @interface 用户点击按钮时调用函数
  @version 1.0 初始的点击功能
           2.0 温度模拟
           2.1 温度模拟修改
 */

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>
#include <QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class MsgUnit
{
public:
    long long refID;
    int temp; //温度
    int wind; //风速
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    int Power;//开关机
    float tmp;//目标温度， 也是用户主要调整的温度参数
    float realTmp;//实时温度 主要是接受服务器的反馈来设定的温度
    float initTmp;//初始温度 初始化时，服务器定下来的温度，是否有用还待存疑
    int speed;  // 当前风速 也是用户主要调整的风速参数
    int kind;//工作模式（冷热风）
    QString roomId;//房间号
    int fee;//费用
    int time_start;//计时参数
    int time_end;
    int flag;
    QString address;//服务器地址和端口

    float Pintval;//时间间隔
    int Pspeed;//用户设定风速
    //原变量
    //画背景图
    void paintEvent(QPaintEvent *);

    //背景亮暗触发函数
    void setting();
private slots:
    /**
     * @brief SimulateTmp 温度模拟的实现
     * @param null
     * @return null
     */
    void SimulateTmp();
    /**
     * @brief gettemp 获取温度
     * @param null
     * @return null
     */
    void gettemp();
    /**
     * @brief on_addTmp_Button_clicked 用户点击升高温度
     * @param null
     * @return null
     */
    void on_addTmp_Button_clicked();
    /**
     * @brief on_reduceTmp_Button_clicked 用户点击降低温度
     * @param null
     * @return null
     */
    void on_reduceTmp_Button_clicked();
    /**
     * @brief on_lowSpeedButton_clicked 用户点击低风
     * @param null
     * @return null
     */
    void on_lowSpeedButton_clicked();
    /**
     * @brief on_middleSpeedButton_clicked 用户点击中风
     * @param null
     * @return null
     */
    void on_middleSpeed_Button_clicked();
    /**
     * @brief on_highSpeed_Button_clicked 用户点击高风
     * @param null
     * @return null
     */
    void on_highSpeed_Button_clicked();
    /**
     * @brief on_windClose_Button_clicked 用户关闭送风
     * @param null
     * @return null
     */
    void on_windClose_Button_clicked();
    /**
     * @brief on_PowerButton_clicked 用户开关机
     * @param checked=1 开机 check=0关机
     * @return null
     */
    void on_PowerButton_clicked(bool checked);
    /**
     * @brief  generate_refId 生成消息id
     * @param null
     * @return 返回响应的id
     */
    QString generate_refId(); //生成refId
    /**
     * @brief open_aircondition 客户端开空调
     * @param null
     * @return null
     */
    void open_aircondition(); //调用关机函数
    /**
     * @brief close_aircondition 客户端关空调
     * @param null
     * @return null
     */
    void close_aircondition(); //调用开机函数
    /**
     * @brief packageAnalyse 客户端解析服务器的报文
     * @param recvJson 服务器向客户端发送的json包
     * @return null
     */
    void packageAnalyse(QJsonObject recvJson);
    /**
     * @brief sendACK 客户端回应服务器
     * @param kind表示回应的种类 refId消息id
     * @return null
     */
    void sendACK(int kind ,long long refId);
    /**
     * @brief sendMsgPackage_timer 定时发送心跳包
     * @param null
     * @return null
     */
    void sendMsgPackage_timer();
    /**
     * @brief sendMsgPackage 客户端向服务器发修改请求
     * @param argqing
     * @return null
     */
    void sendMsgPackage(int arg);
    /**
     * @brief windchange 改变风速
     * @param null
     * @return null
     */
    void windchange(int a);


private:
    Ui::Widget *ui;
    QWebSocket *client;
};
#endif // WIDGET_H
