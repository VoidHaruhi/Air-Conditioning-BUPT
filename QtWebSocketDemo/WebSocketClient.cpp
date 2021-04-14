#include "WebSocketClient.h"
#include "ui_WebSocketClient.h"
#include"QTime"
#include <QTimer>
#include <QHostAddress>
#include <QDebug>
QString *token = new QString("None"); //token
long long randnumber = 1122334455667788; //refID
long long re_randnumber = 1122334455667780; //回收的refID    用于判断请求是否实现
int open_close_flag = 0;
long long open_close_re = 0; //开机关机的refID，最高级别响应
int config_temp = 0; //默认温度
int buttontime = 0; //按钮的冷却时间时间
QTimer * Ptimer = new QTimer(); //模拟环境时间的计时器
QTimer *timer = new QTimer(); //心跳包的间隔时间
QTimer *cdtimer = new QTimer(); //按钮cd的触发函数
MsgUnit* msg_que = new MsgUnit[10]; //消息单元类型的消息队列
int que_top = 0; //头部，标记着第一个未被填写的单元
int que_end = 0; //尾部， 标记这第一个未被回应的单元  top == end //队列空  容量：top>end : end,end+1,...top
int que_limit = 0; //当前容量            top<end : end,end+1,end+2....top-1

QString WebSocketClient::generate_refId()
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

void WebSocketClient::open_aircondition()
{
    Ppower = 1; //开机标志
    Ptimer->start(Pintval);
    timer->start(20000);
    ui->opening->setText("关机");
    ui->spinBox->setEnabled(true);
    ui->spinBox_2->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->groupBox->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->SendButton->setEnabled(true);
    ui->RealTemp->display(Ptmp);
    ui->RealWind->display(1);
    ui->spinBox ->setValue(Ptmp);
    ui->spinBox_2->setValue(1);
}

void WebSocketClient::close_aircondition()
{
    Ppower = 0;
    timer->stop();
    ui->opening->setText("开机");
    ui->spinBox->setEnabled(false);
    ui->spinBox_2->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->groupBox->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->SendButton->setEnabled(false);
    ui->spinBox->setValue(Ptmp);

    ui->RealTemp->display("");
    ui->RealWind->display("");
}

WebSocketClient::WebSocketClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebSocketClient)
{
    ui->setupUi(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(sendMsgPackage_timer()));
    connect(cdtimer,SIGNAL(timeout()),this,SLOT(buttoncd()));

    //进行初始化*******************************************************************
    //timer->start(1000);
    //ui->spinBox_2->setEnabled(false);
    Pintval = 1000;
    PinitTmp = 25;
    PnowTmp = 25;
    Pspeed = 2;
    Pkind = 1;
    Ppower = 0;
    Ptmp = 25;
    Pnowspeed = 0;
    PinitTmp = 25; //室内默认温度先设置为25
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->groupBox->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->makecold->setChecked(true);
    ui->SendButton->setChecked(false);
    //ui->roomTemp->display("00");
    ui->RealTemp->display("");  //这个要根据初始设定温度来决定。
    ui->RealWind->display("");
    client=new QWebSocket;
    client->setParent(this);


    //更改显示环境温度
    Ptimer->start(Pintval);
    connect(Ptimer,&QTimer::timeout,[=](){
        ui->RoomTmp->display(PnowTmp);
        qDebug() <<"初始温度：" <<PinitTmp<<"实时温度："<<PnowTmp<<"目标温度："<<Ptmp<<"风速："<<Pnowspeed<<"目标风速："<<Pspeed;
        SimulateTmp();
    });
    //初始化结束***********************************************************************

    //连接服务端
    connect(ui->btnOpen,&QPushButton::clicked,[this](){
        if(ui->btnOpen->text()!="Open"){
            client->close();
        }else{
            client->open(QUrl(ui->editUrl->text()));

        }
    });


    //连接结果，连上后发送初始化包******************************************************
    ui->btnSend->setEnabled(false);
    connect(client,&QWebSocket::connected,[this](){

        ui->btnSend->setEnabled(true);
        //timer->start(10000);
        ui->btnOpen->setText("Close");
        ui->footLabel->setText(QString("Address:%1  Port:%2")
                               .arg(client->localAddress().toString())
                               .arg(client->localPort()));
        qDebug()<<"connected";

        QJsonObject testobject,data; //创建一个QJsonObject对象
        //randnumber = generate_refId();
        testobject.insert("refId",generate_refId());
        testobject.insert("handler","/client/init");

        data.insert("initTmp",25);
        data.insert("roomId","304");
        testobject.insert("data",data); //测试用
        QJsonDocument document; //
        QByteArray byteArray1 = QJsonDocument(testobject).toJson();
        client->sendTextMessage(byteArray1);
        /*发送初始化包*/
    });
    // 成功发送初始化包 ***************************************************************************


    connect(client,&QWebSocket::disconnected,[this](){
        ui->btnSend->setEnabled(false);
        ui->btnOpen->setText("Open");
        timer->stop();
        qDebug()<<"disconnected";
    });
    //发送数据
    connect(ui->btnSend,&QPushButton::clicked,[this](){
        if(!ui->editSend->toPlainText().isEmpty())
            client->sendTextMessage(ui->editSend->toPlainText());
    });



    //接收数据槽
    connect(client,&QWebSocket::textMessageReceived,[this](const QString &msg){
        ui->editRecv->append(msg);
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

    connect(ui->opening,&QPushButton::clicked,[this](){
        if(ui->opening->text()!="关机"){
            //此时要开机，发送 /client/open包
            open_close_flag = 1;
            open_close_re = generate_refId().toLongLong();
            sendACK(1,open_close_re); //1为open
        }else{
            //改成json包
            open_close_flag = -1;
            open_close_re = generate_refId().toLongLong();
            sendACK(2,open_close_re);
        }
    });
}

WebSocketClient::~WebSocketClient()
{
    //结束的时候没关会异常
    client->close();
    delete ui;
}

// 室内温度模拟模块 ****************************************************************************
void WebSocketClient::SimulateTmp(){
    if(Ppower == 0){//关机 温度要趋近室内默认温度 PinitTmp
        Pnowspeed = 0;
        //ui->RoomTmp->display(P)
        //ui->lineEdit_3->setText(QString::number(PinitTmp));
        //ui->lineEdit_4->setText(QString::number(Pnowspeed));
        if((PnowTmp- PinitTmp)<0.1 && (PnowTmp- PinitTmp)>-0.1)
            Ptimer->stop(); //关机达到平衡时取消计时
        if(PnowTmp > PinitTmp)
        PnowTmp = PnowTmp - 0.5*Pintval/60000;
        else
        PnowTmp = PnowTmp + 0.5*Pintval/60000;
    }
    else {//开机
        //这里只有 设定温度 大于 当前温度    和小于 的情况 制热制冷暂时不考虑  误差0.1附近时可以考虑设置风速为0
        if((PnowTmp- Ptmp)<0.1 && (PnowTmp- Ptmp)>-0.1 && Pnowspeed > 0)
            Pnowspeed = 0; //达到设置温度附近，设置风速为0
        else if ((PnowTmp- Ptmp)>0.1 || (PnowTmp- Ptmp)<-0.1)
            Pnowspeed = Pspeed; //温度差别过大
        //考虑消除震荡
        int k = (PnowTmp > Ptmp)?1:-1 ;
        int k2 = (PnowTmp > PinitTmp)?1:-1;
        qDebug()<<"乘数因子k:"<<k <<" 乘数因子k2"<<k2;
        switch (Pnowspeed) {
        case 0: if((PnowTmp- PinitTmp)>0.1 || (PnowTmp- PinitTmp)<-0.1) PnowTmp = PnowTmp - k2*0.5*Pintval/60000;break;  //风速为0时自动趋近于默认温度
        case 1: PnowTmp = PnowTmp - k*0.5*Pintval/60000*0.8;break;
        case 2: PnowTmp = PnowTmp - k*0.5*Pintval/60000;break;
        case 3: PnowTmp = PnowTmp - k*0.5*Pintval/60000*1.2;break;
        }
    }
}





void WebSocketClient::on_spinBox_valueChanged(int arg1)
{
    qDebug()<<"temp changed"<<arg1;
}


void WebSocketClient::on_spinBox_2_valueChanged(int arg1)
{
}
void WebSocketClient::on_opening_clicked()
{
}
void WebSocketClient::on_opening_clicked(bool checked)
{
}
void WebSocketClient::on_opening_stateChanged(int arg1)
{
}

// 制冷和制热选项似乎现在用不到
void WebSocketClient::on_makecold_clicked(bool checked)
{
}

void WebSocketClient::on_makehot_clicked(bool checked)
{
}


//定时发的包，发送当前的环境温度和token
void WebSocketClient::sendMsgPackage_timer()
{
    QJsonObject testobject,data; //创建一个QJsonObject对象
    testobject.insert("refId",generate_refId()); //refid
    testobject.insert("handler","/client/updateStatus");
    testobject.insert("token",*token);
    data.insert("nowTmp",float(ui->RoomTmp->value()));  //这里应该发当前设定的温度 LCD
    data.insert("roomId","304");
    testobject.insert("data",data);
    ui->spinBox->setValue(ui->RealTemp->value());
    ui->spinBox_2->setValue(ui->RealWind->value());
    QJsonDocument document; //
    QByteArray byteArray1 = QJsonDocument(testobject).toJson();

    client->sendTextMessage(byteArray1);//考虑如何准备接受回应包
}


//点击确定发送的包 用于修改房间温度, power=-1
void WebSocketClient::sendMsgPackage()
{
    QJsonObject testobject,data;
    long long refId = generate_refId().toLongLong();
    testobject.insert("refId",QString::number(refId)); //refId
    testobject.insert("token",*token);
    testobject.insert("handler","/client/controlRoom");
    int temp_a = ui->spinBox->value(); //温度
    int wind_a = ui->spinBox_2->value(); //级数
    data.insert("tmp",float(ui->spinBox->value()));
    data.insert("wind",ui->spinBox_2->value());
    data.insert("power",-1);
    data.insert("roomId","304");
    testobject.insert("data",data);
    QJsonDocument document; //
    QByteArray byteArray1 = QJsonDocument(testobject).toJson();
    //填写消息队列
    if(que_limit == 10){
        //说明大量的包没有得到回复
        qDebug()<<"服务器回应超时";
    }
    else{
        //有空位则填写消息序列 end --- top ,并发送消息
        que_limit++;
        msg_que[que_top].refID = refId;
        msg_que[que_top].temp = temp_a;
        msg_que[que_top].wind = wind_a;
        que_top = (que_top+1)%10;
        client->sendTextMessage(byteArray1);
    }
}

/*客户端收到消息 解析并回复*/

void WebSocketClient::sendACK(int kind ,long long refId)
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
        data.insert("tmp",Ptmp);
        data.insert("wind",1);
        data.insert("power",1);
        data.insert("roomId","304");
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
        //data.insert("tmp",Ptmp);
        //data.insert("wind",1);     关机是否需要发送温度和风速？
        data.insert("power",0);
        data.insert("roomId","304");
        testobject.insert("data",data);
        testobject.insert("token",*token);
        testobject.insert("handler","/client/controlRoom");
        QJsonDocument document; //
        QByteArray byteArray1 = QJsonDocument(testobject).toJson();
        client->sendTextMessage(byteArray1);//考虑如何准备接受回应包
    }
    else;
}

void WebSocketClient::packageAnalyse(QJsonObject recvJson)
{
    long long refId = recvJson["refId"].toString().toLongLong();
    QString handler = recvJson["handler"].toString();
    //开房请求
    if(handler == "/server/openRoom"){
        QJsonObject data = recvJson.find("data")->toObject();
        QString a = data["token"].toString();
        qDebug()<<"token:"<<token;
        *token = a;   //更新token
        sendACK(0,refId);
        int temp = data["defaultTmp"].toDouble();    //默认温度？，保存在
        ui->RealTemp->display(temp);
        //ui->RealWind->display(1);   //默认风速？
        //设置温度和风速
        config_temp = temp; //设置默认温度
        Ptmp = float(temp);
    }
    //发送set请求后的回复
    else if(handler == "/server/confirm"){
        //要回应消息序列的  考虑refID小的需不需要回应
        //最先回应开关机请求，调用开关机函数
        QString retemp;
        retemp = recvJson["refId"].toString();
        re_randnumber = retemp.toLongLong();
        //re_randnumber = recvJson["refId"].toInt();
        qDebug()<<"收到的refID"<<re_randnumber;
        //响应开关机请求
        if(open_close_re != 0){
            if(re_randnumber == open_close_re){
                //成功响应
                if(open_close_flag == 1){
                    //调用开机函数
                    open_aircondition();
                }
                else if(open_close_flag == -1){
                    //调用关机函数
                    close_aircondition();
                }
                else{
                    qDebug()<<"未修改open_close_flag";
                }
                //清空消息队列
                que_limit = 0;
                que_end = 0;
                que_top = 0;
                open_close_re = 0;
                open_close_flag = 0;
            }
            else;
        }

        for(int i=0;i<que_limit;i++){
            //遍历消息队列寻找消息
            qDebug()<<"现有的的refID"<<re_randnumber;
            if(msg_que[(i+que_end)%10].refID == re_randnumber){
                //找到目标信息，进行温度与风速显示的修改，并修改队列的信息,抛弃在它之前未被回应的包。
                ui->RealTemp->display(msg_que[(i+que_end)%10].temp);
                ui->RealWind->display(msg_que[(i+que_end)%10].wind);
                Ptmp = msg_que[(i+que_end)%10].temp;
                Pspeed = msg_que[(i+que_end)%10].wind;
                que_limit = que_limit-(i+1);
                que_end = (i+que_end)%10 + 1;

            }
            else;
        }
    }

    else if(handler == "/server/setRoom"){
        QJsonObject data = recvJson.find("data")->toObject();
        QString a = data["token"].toString();

        double temp = data["tmp"].toDouble();
        int wind = data["wind"].toInt();

        sendACK(0,refId);
        int power = data["power"].toInt();
        qDebug()<<"power:"<<power;
        if(power == 1){
            //开机，而且要设置空调的温度和风速
            Ppower = 1;
            if( ui->opening->text() == "开机"){
                open_aircondition();
            }
            else{
                qDebug()<<"空调已经开启";
            }
            if(temp != -1){
                //温度设置
                ui->RealTemp->display(temp);
                Ptmp = temp;  //从关机到开机
            }
            else{
                ui->RealTemp->display(config_temp); //回复默认温度
                Ptmp = config_temp;
            }
            if(wind != -1){
                ui->RealWind->display(wind);
                Pspeed = wind;
            }
            else{
                ui->RealWind->display(1);
                Pspeed = 1;  //默认为1
            }
        }
        else if(power == -1){
            //保持不变,开机情况下修改温度和风速
            if(ui->opening->text() == "关机"){
                //此时已经开机
                if(temp != -1){
                    //温度设置
                    ui->RealTemp->display(temp);
                    Ptmp = temp;  //从关机到开机
                }
                else;
                if(wind != -1){
                    ui->RealWind->display(wind);
                    Pspeed = wind;
                }
            }
            else{
                //关机状态下就不管了
            }
        }
        else{
            //关机
            Ppower = 0;
            if(ui->opening->text()=="关机"){
                //Ptimer->stop();
                close_aircondition();
                Pspeed = 0;
                timer->stop();
            }
            else{
                qDebug() <<"空调已经关闭";
            }
        }
    } //结束 setRoom的解析
    else{
        qDebug()<<"未解析包格式:"<<handler;
    }
    qDebug()<<handler;
    qDebug()<<refId;
}



void WebSocketClient::on_SendButton_clicked()
{
    //清空全局定时器
    timer->stop();
    sendMsgPackage(); //发包，希望收到回复，回复后才能设置空调参数
    ui->SendButton->setEnabled(false);
    ui->SendButton->setText("确认(3s)");
    buttontime = 3;
    timer->start(20000);
    cdtimer->start(1000);
    //希望收到回复

}
void WebSocketClient::buttoncd()
{
    buttontime--;
    if(buttontime == 0){
        cdtimer->stop();
        ui->SendButton->setEnabled(true);
        ui->SendButton->setText("确认");
    }
    else{
        QString a = "确认(";
        QString a2 = QString::number(buttontime);
        a = a + a2+ "s)";
        qDebug()<<a;
        ui->SendButton->setText(a);
    }
}
