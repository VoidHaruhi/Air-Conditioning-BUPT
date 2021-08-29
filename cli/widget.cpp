/**
  @title widget.cpp
  @module 实现客户端与用户交互的功能，实现与服务器交互的功能，实现温度模拟功能
  @author 李文淇
  @version 1.0 初始的点击功能
           1.1 沟通服务器
           2.0 温度模拟

 */



#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QIcon>
#include <QTimer>
#include <QDateTime>
#include <qinputdialog.h>
#include <QDebug>
#include <QHostAddress>
#include <QPainter>
#include <QDebug>
using namespace std;
int P = 1;
double templow = 0,temphigh = 0;
QString url_add ="xx.xx.xx.xx:xxxx"; //地址，以后开机直接链接这个就行
QString *token = new QString("None"); //token
int OpenFlag = 0;  //0为关机 1为开机 用来取代标签上的数字
long long randnumber = 1122334455667788; //refID
long long re_randnumber = 1122334455667780; //回收的refID    用于判断请求是否实现
int open_close_flag = 0;
long long open_close_re = 0; //开机关机的refID，最高级别响应
double config_temp = 0; //默认温度
int config_wind = 0; //默认风速
QTimer * Ptimer = new QTimer(); //模拟环境时间的计时器
QTimer *massage_timer = new QTimer(); //心跳包的间隔时间
MsgUnit* msg_que = new MsgUnit[10]; //消息单元类型的消息队列
int que_top = 0; //头部，标记着第一个未被填写的单元
int que_end = 0; //尾部， 标记这第一个未被回应的单元  top == end //队列空  容量：top>end : end,end+1,...top
int que_limit = 0; //当前容量            top<end : end,end+1,end+2....top-1
int prespeed = 0; //未响应前的speed
int lastspeed = 0;
int pretemp = 0; //未响应前的temp
int returnflag = 0;
//重写paintEvent来显示背景图片

QString Widget::generate_refId()
{
    QTime *time = new QTime() ;

    QString ref ;
    //random_device rd;
    ref = time->currentTime().toString ( "hhmmsszzz") ;
    //int i = QRandomGenerator::global()->bounded(100) ;
    qsrand(time->msec()+time->second()*1000);
    int i = qrand()%100;
    if(i<=9)
    ref = ref+QString::number(0) + QString::number(i);
    else ref = ref+QString::number(i) ;
    qDebug()<<ref<<ref.toInt();
    return ref ;
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //this->setWindowFlags(Qt::FramelessWindowHint);
    connect(massage_timer,SIGNAL(timeout()),this,SLOT(sendMsgPackage_timer()));
    client=new QWebSocket;
    client->setParent(this);
    //设置初始值
    Pintval = 1000; //温度模拟间隔1000ms
    kind = 0;
    Power = 0;
    tmp = 25;
    realTmp = 25.0;
    initTmp = 25;
    flag = 0;
    speed = 2;
    bool ok;
    //输入服务器端口和地址
    QString addressText = QInputDialog::getText(this, tr("服务器地址和端口输入"),tr("请输入服务器地址和端口"), QLineEdit::Normal,"ws://localhost:12345", &ok);
    if (ok && !addressText.isEmpty())
        address = addressText;
    qDebug()<<address<<endl;
    roomId = "304d";
    QString romid = QInputDialog::getText(this, tr("房间号输入"),tr("房间号输入"), QLineEdit::Normal,"303d", &ok);
    if (ok && !romid.isEmpty())
        roomId = romid;
    qDebug()<<roomId<<endl;
    //输入房间号
    QString tmpText = QInputDialog::getText(this, tr("初始温度设置"),tr("初始温度设置"), QLineEdit::Normal,"28", &ok);
    if (ok && !addressText.isEmpty())
        realTmp = tmpText.toFloat();
    QString plus = QInputDialog::getText(this, tr("倍率设置"),tr("倍率设置"), QLineEdit::Normal,"1", &ok);
    if (ok && !addressText.isEmpty())
        P = plus.toInt();
    initTmp = realTmp;
    qDebug()<<realTmp<<endl;
   client->open(QUrl(address));
    //连接结果，连上后发送初始化包******************************************************

    connect(client,&QWebSocket::connected,[this](){
        //timer->start(10000);
        //ui->btnOpen->setText("Close");
        qDebug()<<"connected";

        QJsonObject testobject,data; //创建一个QJsonObject对象
        //randnumber = generate_refId();
        testobject.insert("refId",generate_refId());   //refid
        testobject.insert("handler","/client/init");   //handler

        data.insert("initTmp",initTmp);                     //data - iniTmp
        data.insert("roomId",roomId);                    //data - roomId
        testobject.insert("data",data); //测试用
        QJsonDocument document; //
        QByteArray byteArray1 = QJsonDocument(testobject).toJson();
        client->sendTextMessage(byteArray1);
        //发送初始化包
    });

     // 成功发送初始化包 ***************************************************************************

    //绑定接受槽
    connect(client,&QWebSocket::textMessageReceived,[this](const QString &msg){
        qDebug()<<"received";
        QByteArray recvPackage = msg.toUtf8();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(recvPackage, &parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug()<<"解析json文件错误.";
            return;
        }
        packageAnalyse(document.object());
    });

    //温度模拟

    Ptimer->start(Pintval);
    connect(Ptimer,&QTimer::timeout,[=](){
        ui->realTmp_valuelabel->setText(QString::number(realTmp));
        //qDebug() <<"初始温度：" <<PinitTmp<<"实时温度："<<PnowTmp<<"目标温度："<<Ptmp<<"风速："<<Pnowspeed<<"目标风速："<<Pspeed;
        SimulateTmp();
    });
    ui->currentfee_label->setText("0.0");
    ui->fee_valuelabel->setText("0.0");
    //显示实时温度
    QString str = QString::number(realTmp, 'f', 1);
    ui->realTmp_valuelabel->setText(str);
    ui->targetTmp_valuelabel->setText("--");
    //计时器显示时间
    QTimer * timer = new QTimer(this);
    timer->start(1000);
    connect(timer,&QTimer::timeout,[=](){
        time_end = (int)clock();
       // qDebug()<<(time_end - time_start)/1000;
        if((time_end - time_start)/1000 >= 10){
            flag = 0;
            update();
        }
        QString current_date =QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ddd");
        ui->time_label->setText(current_date);
    });

    //设置初始界面
    QPixmap pix1;//风速等级初始值
    pix1.load(":/pictures/wind0.png");
    ui->wind_label_2->setPixmap(pix1);
    ui->wind_label_2->setScaledContents(true);
    ui->wind_label_2->show();




    //设置窗口固定大小
    setFixedSize(699,557);

    //设置窗口图标
    setWindowIcon(QIcon(":/pictures/title.png"));
    //设置标题
    setWindowTitle(roomId+"号房间空调界面");


}

void Widget::paintEvent(QPaintEvent *){

    QPainter painter(this);
    QPixmap pix;
    if(flag == 0)
        pix.load(":/pictures/background1.jpg");
    else
        pix.load(":/pictures/background.jpg");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);



}

// 室内温度模拟模块 ****************************************************************************
void Widget::SimulateTmp(){
    if(Power == 0){//关机 温度要趋近室内默认温度 PinitTmp
        if((realTmp - initTmp)<0.1 && (realTmp - initTmp)>-0.1) // realTmp - PnowTmp     initTmp - PinitTmp
        {
            Ptimer->stop(); //关机达到平衡时取消计时
        }
        if(realTmp > initTmp)
        realTmp = realTmp - P*0.5*Pintval/60000;
        else
        realTmp = realTmp + P*0.5*Pintval/60000;
    }
    else {//开机
        //这里只有 设定温度 大于 当前温度    和小于 的情况 制热制冷暂时不考虑  误差1附近时可以考虑设置风速为0
        if(kind == 0){
            //制冷
            if(realTmp < tmp && speed > 0){
                if(returnflag == 0){
                    lastspeed = speed;
                    sendACK(3,generate_refId().toLongLong());
                    returnflag = 1;
                }
                else;
                speed = 0; //达到设置温度附近，设置风速为0  -- 调用设置风速模块
                windchange(0);
            }

            else if (returnflag == 1 && (realTmp - 1 > tmp))
            {
                //回温到现实温度大于设定温度+1后，申请释放
                //realTmp = tmp + 1;
                sendACK(4,generate_refId().toLongLong());//申请release
                returnflag = 0;
            }
            //考虑消除震荡
            int k = (realTmp > tmp)?1:-1 ;

            switch (speed) {
            //根据风速来设定
            case 0: if(realTmp < initTmp) realTmp = realTmp + P*0.5*Pintval/60000;break;  //风速为0时自动趋近于默认温度
            case 1: realTmp = realTmp - P*k*0.33333*Pintval/60000;break;     // 风速1  1/180度 每秒
            case 2: realTmp = realTmp - P*k*0.5*Pintval/60000;break;        // 风速2  1/120度 每秒
            case 3: realTmp = realTmp - P*k*Pintval/60000;break;     // 风速3  1/60度 每秒
            }
        }
        else if(kind == 1){
            //制热
            if(realTmp > tmp && speed > 0){
                if(returnflag == 0){
                    lastspeed = speed;
                    sendACK(3,generate_refId().toLongLong());
                    returnflag = 1;
                }
                else;
                speed = 0; //达到设置温度附近，设置风速为0  -- 调用设置风速模块
                windchange(0);
            }
            else if (returnflag == 1 && (realTmp + 1 < tmp))
            {
                //回温到现实温度小于设定温度-1后，申请释放
                //realTmp = tmp + 1;
                sendACK(4,generate_refId().toLongLong());//申请release
                returnflag = 0;
            }
            //考虑消除震荡
            int k = (realTmp > tmp)?1:-1 ;
            int k2 = (realTmp > initTmp)?1:-1;

            switch (speed) {
            //根据风速来设定
            case 0: if(realTmp > initTmp) realTmp = realTmp - P*0.5*Pintval/60000;break;  //风速为0时自动趋近于默认温度
            case 1: realTmp = realTmp - P*k*0.33333*Pintval/60000;break;     // 风速1  1/180度 每秒
            case 2: realTmp = realTmp - P*k*0.5*Pintval/60000;break;        // 风速2  1/120度 每秒
            case 3: realTmp = realTmp - P*k*Pintval/60000;break;     // 风速3  1/60度 每秒
            }
        }
        else;
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setting(){
    flag = 1;
    update();
    time_start = (int)clock();
}
//升温
void Widget::on_addTmp_Button_clicked()
{
    // Power为开机标志
    qDebug()<<"temphigh: "<<temphigh<<endl;
    if(Power == 1){
        setting();
        if(tmp>=temphigh)
            tmp--;
        ui->targetTmp_valuelabel->setText(QString::number(++tmp));
        if(returnflag==0)sendMsgPackage(1);
    }

}

//降温
void Widget::on_reduceTmp_Button_clicked()
{
    qDebug()<<"templow: "<<templow<<endl;
    if(Power == 1){
        setting();
        if(tmp<=templow)
            tmp++;
        ui->targetTmp_valuelabel->setText(QString::number(--tmp));
        if(returnflag==0)sendMsgPackage(1);
    }
}
//风速为1
void Widget::on_lowSpeedButton_clicked()
{

    if(Power == 1){
        setting();
        prespeed = 1;
        lastspeed = 1;
        if(returnflag==0)sendMsgPackage(2);/*只有非回温时才需要控制*/
    }
}
//风速为2
void Widget::on_middleSpeed_Button_clicked()
{
    if(Power == 1){
        setting();
        prespeed = 2;
        lastspeed = 2;
        if(returnflag==0)sendMsgPackage(2);/*只有非回温时才需要控制*/
    }

}
//风速为3
void Widget::on_highSpeed_Button_clicked()
{
    if(Power == 1){
        setting();
        QPixmap pix;
        prespeed = 3;
        lastspeed = 3;
        if(returnflag==0)sendMsgPackage(2);/*只有非回温时才需要控制*/
    }

}
//关闭送风
void Widget::on_windClose_Button_clicked()
{
    if(Power == 1){
        setting();
        speed = 0;
        lastspeed=0;
        if(returnflag==0)sendMsgPackage(2);/*只有非回温时才需要控制*/
    }

}


void Widget::windchange(int a)
{
    QPixmap pix2;//风速等级

    if(a == 0) pix2.load(":/pictures/wind0.png");
    if(a == 1) pix2.load(":/pictures/wind1.png");
    if(a == 2) pix2.load(":/pictures/wind2.png");
    if(a == 3) pix2.load(":/pictures/wind3.png");
    ui->wind_label_2->setPixmap(pix2);
    ui->wind_label_2->setScaledContents(true);
    ui->wind_label_2->show();
}


// 开机按钮按下 区别与此时的状态 注意实际运行时，按下按钮仅发送开机指令，收到回复后才可以调用开机按钮
void Widget::on_PowerButton_clicked(bool checked)
{
    // 根据当前的情况判断，如果是开机，则一直发送关机请求，防止响应不及时或丢包关系，与是否checked无关
    if(OpenFlag == 0){
        //此时要开机，发送 /client/open包
        open_close_flag = 1;   // open_close_flag = 1 ,说明收到的回复是针对开机报文
        open_close_re = generate_refId().toLongLong();//open_close_re记录了发送开关机请求的包
        sendACK(1,open_close_re); //1为open
    }else{
        //改成json包
        open_close_flag = -1;
        open_close_re = generate_refId().toLongLong();
        close_aircondition();
        sendACK(2,open_close_re);
    }

}


void Widget::open_aircondition()
{
    ui->currentfee_label->setText("0.0");
    ui->PowerButton->setChecked(1);
    massage_timer->start(int(6000/P));
    Ptimer->start(Pintval);
    returnflag = 0;
    OpenFlag = 1;
    Power = 1;
    flag = 1;
    update();
    time_start = (int)clock();
    QPixmap pix1;
    ui->targetTmp_valuelabel->setText(QString::number(tmp));   //设置好tmp
    if(kind == 1)
        pix1.load(":/pictures/hot.png"),ui->label->setText("hot");   //设置好kind
    else
        pix1.load(":/pictures/cool.png"),ui->label->setText("cool");
    ui->work_label->setPixmap(pix1);
    ui->work_label->setScaledContents(true);
    ui->work_label->show();

    ui->targetTmp_valuelabel->setText(QString::number(config_temp)); //设定缺省温度
    windchange(config_wind);
}

void Widget::close_aircondition()
{
    //关机清零  开机会设置
    massage_timer->stop();
    ui->PowerButton->setChecked(0);
    config_temp = tmp;
    if(speed != 0) config_wind = speed;
    OpenFlag = 0;
    Power = 0;
    flag = 0;
    update();
    ui->targetTmp_valuelabel->setText("--");
    ui->work_label->clear();//工作模式清零
    ui->label->clear();//工作模式清零
    speed = 0;
    QPixmap pix;//风速等级清零
    pix.load(":/pictures/wind0.png");
    ui->wind_label_2->setPixmap(pix);
    ui->wind_label_2->setScaledContents(true);
    ui->wind_label_2->show();
}



void Widget::sendMsgPackage(int arg)
{
    QJsonObject testobject,data;
    long long refId = generate_refId().toLongLong();
    testobject.insert("refId",QString::number(refId)); //refId
    testobject.insert("token",*token); //token
    testobject.insert("handler","/client/controlRoom"); //handler
    if(arg == 1){
        //调温
        data.insert("tmp",float(tmp));
        data.insert("wind",-1);
    }
    else if(arg == 2){
        //调风
        data.insert("wind",prespeed);
        data.insert("tmp",-1);
    }
    else;
    data.insert("power",-1);   // ps 通过这种途径修改的都大于1吧
    data.insert("roomId",roomId);
    testobject.insert("data",data);
    QJsonDocument document; //
    QByteArray byteArray1 = QJsonDocument(testobject).toJson();
    client->sendTextMessage(byteArray1);  //新版要求，直接发送即可
}



void Widget::sendMsgPackage_timer()
{
    // 如果关机 收到回复包后仅更新费用和
    QJsonObject testobject,data; //创建一个QJsonObject对象
    testobject.insert("refId",generate_refId()); //refid
    testobject.insert("handler","/client/requestState"); //handler
    testobject.insert("token",*token); //token
    data.insert("roomId",roomId); //data
    data.insert("currentTmp",realTmp); //环境温度
    testobject.insert("data",data);
    QJsonDocument document;
    QByteArray byteArray1 = QJsonDocument(testobject).toJson();
    client->sendTextMessage(byteArray1);//准备接受服务器的回复包  handler = /server/update
}


void Widget::sendACK(int kind ,long long refId)
{
    if(kind == 0){
        //普通ack
        QJsonObject testobject,data; //创建一个QJsonObject对象
        testobject.insert("refId",QString::number(refId));

        testobject.insert("token",*token);
        testobject.insert("handler","/client/confirm");
        QJsonDocument document; //
        QByteArray byteArray1 = QJsonDocument(testobject).toJson();

        client->sendTextMessage(byteArray1);//考虑如何准备接受回应包
    }
    else if(kind == 1){
        //发送开机申请 /client/open power==1
        QJsonObject testobject,data; //创建一个QJsonObject对象
        testobject.insert("refId",QString::number(refId));
        data.insert("tmp",config_temp);
        data.insert("wind",config_wind);
        data.insert("power",1);
        data.insert("roomId",roomId);
        testobject.insert("data",data);
        testobject.insert("token",*token);
        testobject.insert("handler","/client/controlRoom");
        QJsonDocument document; //
        QByteArray byteArray1 = QJsonDocument(testobject).toJson();

        client->sendTextMessage(byteArray1);//考虑如何准备接受回应包
    }
    else if(kind == 2){
        //发送关机申请
        QJsonObject testobject,data; //创建一个QJsonObject对象
        testobject.insert("refId",QString::number(refId));
        data.insert("tmp",config_temp);
        data.insert("wind",config_wind);     //关机是否需要发送温度和风速？
        data.insert("power",0);
        data.insert("roomId",roomId);
        testobject.insert("data",data);
        testobject.insert("token",*token);
        testobject.insert("handler","/client/controlRoom");
        QJsonDocument document; //
        QByteArray byteArray1 = QJsonDocument(testobject).toJson();
        client->sendTextMessage(byteArray1);//考虑如何准备接受回应包
    }
    else if(kind == 3 || kind == 4){
        //回温
        QJsonObject testobject,data; //创建一个QJsonObject对象
        testobject.insert("refId",QString::number(refId));
        data.insert("currentTmp",realTmp);
        data.insert("roomId",roomId);
        testobject.insert("token",*token);
        if(kind == 3)testobject.insert("handler","/client/tempUp");
        if(kind == 4){
            testobject.insert("handler","/client/tempRelease");
            data.insert("wind",lastspeed);
            data.insert("tmp",tmp);
        }
        testobject.insert("data",data);
        QJsonDocument document; //
        QByteArray byteArray1 = QJsonDocument(testobject).toJson();
        client->sendTextMessage(byteArray1);//考虑如何准备接受回应包
    }
    else;
}

void Widget::gettemp()
{
    if(roomId == "304d"){
        if(kind == 1) realTmp = initTmp = 19;
        if(kind == 0) realTmp = initTmp = 29;
    }
    if(roomId == "303d"){
        if(kind == 1) realTmp = initTmp = 18;
        if(kind == 0) realTmp = initTmp = 27;
    }
    if(roomId == "302d"){
        if(kind == 1) realTmp = initTmp = 15;
        if(kind == 0) realTmp = initTmp = 28;
    }
    if(roomId == "301d"){
        if(kind == 1) realTmp = initTmp = 16;
        if(kind == 0) realTmp = initTmp = 32;
    }
}

void Widget::packageAnalyse(QJsonObject recvJson)
{
    qDebug()<< recvJson;
    long long refId = recvJson["refId"].toString().toLongLong();
    QString handler = recvJson["handler"].toString();
    //解析开房请求 6.14
    if(handler == "/server/openRoom"){
        QJsonObject data = recvJson.find("data")->toObject();
        QString a = data["token"].toString();
        qDebug()<<"token:"<<token;
        *token = a;   //更新token

        templow = data["tempLow"].toDouble();
        temphigh = data["tempHigh"].toDouble();

        kind = data["mode"].toInt(); //制冷or制热

        config_temp = data["defaultTmp"].toDouble();
        config_wind = data["defaultWind"].toInt();

        sendACK(0,refId);
            //默认温度？，看看保存在哪里合适
        //gettemp();
        ui->realTmp_valuelabel->setText(QString::number(realTmp));
        //initTmp = config_temp;
    }
    // 2021.6.14 新增sever/change
    else if(handler == "/server/change"){
        QJsonObject data = recvJson.find("data")->toObject();
        double changetmp = data["tmp"].toDouble();
        int changewind = data["wind"].toInt();

        //开关机： 唯一的开关机
        int power = data["power"].toInt();
        if(power == 1 || (power == -1 && Power == 1)){
            if(changetmp >= 0){
                tmp = changetmp;  //设置温度
                //ui->targetTmp_valuelabel->setText(QString::number(tmp));
            }
            if(power == 1 && Power == 0){
                //主动开机
                open_aircondition();  //调用开机函数 设置具体信息
            }
            Power = 1;
            // 开机完毕 调整参数
            ui->targetTmp_valuelabel->setText(QString::number(tmp));

            if(changewind >= 0){
                windchange(changewind);
                if(speed > 0 && changewind == 0){
                    //空调被调停 打印信息 todo
                    setWindowTitle(roomId+"号房间因中央空调超负荷，暂时无法提供服务");
                }
                else if(changewind > 0){
                    setWindowTitle(roomId+"号房间空调界面");

                }
                else;
                speed = changewind;
                config_wind = speed;
            }
        }

        else{
            //关机或保持关机
            close_aircondition();
        }


    }
    else if(handler == "/server/update"){
        QJsonObject data = recvJson.find("data")->toObject();
        double curfee = data["currentFee"].toDouble();
        double totfee = data["totalFee"].toDouble();
        //todo 显示curfee 和tofee
        //double fee = data["totalFee"].toDouble();
        QString a = QString::number(totfee);
        ui->fee_valuelabel->setText(a);
        QString b = QString::number(curfee);
        ui->currentfee_label->setText(b);

    }
    else if(handler == "/server/tempRelease"){
        QJsonObject data = recvJson.find("data")->toObject();
        int wind = data["wind"].toInt();
        speed = wind;
        windchange(speed);
    }
    else if(handler == "/server/confirm"){
        //
    }
    else{
        qDebug()<<"未解析包格式:"<<handler;
    }

}
