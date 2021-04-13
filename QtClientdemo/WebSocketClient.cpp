#include "WebSocketClient.h"
#include "ui_WebSocketClient.h"
#include <QTimer>
#include <QHostAddress>
#include <QDebug>
QString *token = new QString("None"); //token
long long randnumber = 1122334455667788; //refID
long long re_randnumber = 1122334455667780; //回收的refID    用于判断请求是否实现
int buttontime = 0; //按钮的冷却时间时间
QTimer * Ptimer = new QTimer(); //模拟环境时间的计时器
QTimer *timer = new QTimer(); //心跳包的间隔时间
QTimer *cdtimer = new QTimer(); //按钮cd的触发函数
MsgUnit* msg_que = new MsgUnit[10]; //消息单元类型的消息队列
int que_top = 0; //头部，标记着第一个未被填写的单元
int que_end = 0; //尾部， 标记这第一个未被回应的单元  top == end //队列空  容量：top>end : end,end+1,...top
int que_limit = 0; //当前容量            top<end : end,end+1,end+2....top-1

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
    Ptmp = 26;
    Pnowspeed = 0;
    PinitTmp = 25; //室内默认温度先设置为25
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->groupBox->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->makecold->setChecked(true);
    //ui->roomTemp->display("00");
    ui->RealTemp->display(PinitTmp);  //这个要根据初始设定温度来决定。
    client=new QWebSocket;
    client->setParent(this);


    //更改显示环境温度
    Ptimer->start(Pintval);
    connect(Ptimer,&QTimer::timeout,[=](){
        //QString current_date =QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ddd");

        //ui->label->setText(current_date);
        ui->RoomTmp->display(PnowTmp);
        //ui->lineEdit->setText(QString::number(nowTmp));
        //ui->lineEdit_2->setText(QString::number(nowspeed));

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
        long long refId = randnumber;
        testobject.insert("refId",QString::number(refId));
        randnumber++;

        testobject.insert("handler","/client/init");

        data.insert("initTmp",25);
        data.insert("roomId",304);
        testobject.insert("data",data); //测试用
        //packageAnalyse(testobject);
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
        //调用解析函数解析msg
        //while(msg != ""){
            //qDebug() <<"[New Package] From server :";
            //qDebug() << msg << endl;

            // 进行包的解析
            QByteArray recvPackage = msg.toUtf8();
            QJsonParseError parseJsonErr;
            QJsonDocument document = QJsonDocument::fromJson(recvPackage, &parseJsonErr);
            if(!(parseJsonErr.error == QJsonParseError::NoError))
            {
                qDebug()<<"解析json文件错误.";
                return;
            }
            packageAnalyse(document.object());

        //}
    });


    connect(ui->opening,&QPushButton::clicked,[this](){
        if(ui->opening->text()!="关机"){
            //此时要开机，发送 /client/open包

            sendACK(1,randnumber); //1为open
            randnumber++;
            Ppower = 1; //开机标志
            Ptimer->start(Pintval);
            timer->start(20000);
            //client->sendTextMessage("room xx Openning");

            ui->opening->setText("关机");
            ui->spinBox->setEnabled(true);
            ui->spinBox_2->setEnabled(true);
            //ui->spinBox_2->setEnabled(true);
            ui->pushButton_2->setEnabled(true);
            ui->pushButton_3->setEnabled(true);
            ui->groupBox->setEnabled(true);
            ui->pushButton_3->setEnabled(true);
        }else{
            //改成json包
            sendACK(2,randnumber);
            randnumber++;
            //Ptimer->stop();
            Ppower = 0;
            timer->stop();
            ui->opening->setText("开机");
            //client->sendTextMessage("room xx Closeing");
            ui->spinBox->setEnabled(false);
            ui->spinBox_2->setEnabled(false);
            ui->pushButton_2->setEnabled(false);
            ui->pushButton_3->setEnabled(false);
            ui->groupBox->setEnabled(false);
            ui->pushButton_3->setEnabled(false);
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
    if(checked)
    {
        qDebug()<<"Make cold" << int(ui->makehot->isChecked());
    }
}

void WebSocketClient::on_makehot_clicked(bool checked)
{
    if(checked)
    {
        qDebug()<<"Make hot" << ui->makehot->isChecked();
        //client->sendTextMessage("set hot");
    }
}

void test0()
{
    qDebug("触发信号test0");
}
//定时发的包，发送当前的环境温度和token
void WebSocketClient::sendMsgPackage_timer()
{
    QJsonObject testobject,data; //创建一个QJsonObject对象
    long long temp = randnumber;
    testobject.insert("refId",QString::number(temp));
    randnumber++;
    testobject.insert("handler","/client/updateStatus");
    data.insert("nowTmp",ui->RoomTmp->value());  //这里应该发当前设定的温度 LCD
    data.insert("token",*token);
    testobject.insert("data",data); //测试用
    ui->spinBox->setValue(ui->RealTemp->value());
    ui->spinBox_2->setValue(ui->RealWind->value());
    //packageAnalyse(testobject);
    QJsonDocument document; //
    QByteArray byteArray1 = QJsonDocument(testobject).toJson();

    client->sendTextMessage(byteArray1);//考虑如何准备接受回应包
}


//点击确定发送的包 用于修改房间温度
void WebSocketClient::sendMsgPackage()
{
    QJsonObject testobject,data; //创建一个QJsonObject对象
    long long refId = randnumber;
    testobject.insert("refId",QString::number(refId));
    randnumber++;
    testobject.insert("token",*token);
    testobject.insert("handler","/client/set");
    int temp_a = ui->spinBox->value(); //温度
    int wind_a = ui->spinBox_2->value(); //级数
    data.insert("tmp",ui->spinBox->value());
    data.insert("wind",ui->spinBox_2->value());
    testobject.insert("data",data); //测试用
    //packageAnalyse(testobject);
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

        testobject.insert("token",*token);
        testobject.insert("handler","/client/open");
        testobject.insert("power",1);
        QJsonDocument document; //
        QByteArray byteArray1 = QJsonDocument(testobject).toJson();

        client->sendTextMessage(byteArray1);//考虑如何准备接受回应包
    }
    else if(kind == 2){
        //发送关机申请
    }
    else;
}

void WebSocketClient::packageAnalyse(QJsonObject recvJson)
{
    long long refId = recvJson["refId"].toString().toLongLong();
    QString handler = recvJson["handler"].toString();
    if(handler == "/server/openRoom"){
        //QJsonObject data ;
        //data = QJsonObject(recvJson["data"]); //设置并保存token，以后需要用到
         QJsonObject data = recvJson.find("data")->toObject();
        QString a = data["token"].toString();
         qDebug()<<"token:"<<token;
        *token = a;
        sendACK(0,refId);
       //Tmp = recvJson["defaultTmp"]
        //token = new QString(a); //更新token

        int temp = data["defaultTmp"].toInt();
        //int wind = data["wind"].toInt();
        ui->RealTemp->display(temp);
        //ui->RealWind->display(wind);
        //设置温度和风速
        Ptmp = temp;
        //Pspeed = wind;
        /*回复ACK*/


    }
    //发送set请求后的回复
    else if(handler == "/server/confirm"){
        //要回应消息序列的  考虑refID小的需不需要回应
        QString retemp;
        retemp = recvJson["refId"].toString();
        re_randnumber = retemp.toLongLong();
        //re_randnumber = recvJson["refId"].toInt();
        qDebug()<<"收到的refID"<<re_randnumber;
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
        //sendACK(0,)
        QJsonObject data = recvJson.find("data")->toObject();
        QString a = data["token"].toString();

        int temp = data["tmp"].toInt();
        int wind = data["wind"].toInt();

        sendACK(0,refId);

        QString power = data["power"].toString();
        if(power == "on"){
            if( ui->opening->text() == "开机"){
                Ptimer->start(Pintval);
                ui->opening->setText("关机");
                ui->spinBox->setEnabled(true);
                ui->spinBox_2->setEnabled(true);
                //ui->spinBox_2->setEnabled(true);
                ui->pushButton_2->setEnabled(true);
                ui->pushButton_3->setEnabled(true);
                ui->groupBox->setEnabled(true);
                ui->pushButton_3->setEnabled(true);
                ui->RealTemp->display(temp);
                ui->RealWind->display(wind);
                Ptmp = temp;
                Pspeed = wind;
                timer->start(20000);
            }
            else{
                qDebug()<<"空调已经开启";
            }
        }
        else{
            //关机
            if(ui->opening->text()=="关机"){
                Ptimer->stop();
                ui->opening->setText("开机");

                ui->spinBox->setEnabled(false);
                ui->spinBox_2->setEnabled(false);
                ui->pushButton_2->setEnabled(false);
                ui->pushButton_3->setEnabled(false);
                ui->groupBox->setEnabled(false);
                ui->pushButton_3->setEnabled(false);
                ui->RealTemp->display(00);
                ui->RealWind->display(00);
                Ptmp = 0;
                Pspeed = 0;
                timer->stop();
            }
            else{
                qDebug() <<"空调已经关闭";
            }
        }
    }
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
