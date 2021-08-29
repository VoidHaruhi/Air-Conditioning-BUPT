#include "widget.h"
#include "ui_widget.h"
/**
* @projectName   SEMpro2
* @brief         主要界面的源文件实现 包含各种发包函数 收包处理函数 槽函数 生成报表调用功能 界面控件操作显示逻辑实现函数
* @author        Oliver
* @date          2021-06-30
*/
Widget::Widget (QString token_local,QWidget *parent,int type)
    : QWidget(parent),token(token_local)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    IniWidget();
}
//初始化Widget内容 设置控件参数
void Widget::IniWidget(){

    setAutoFillBackground(true);
    QPalette palette;
    QPixmap pixmap(":/bk.png");
    palette.setBrush(QPalette::Window, QBrush(pixmap));
    setPalette(palette);
    setFixedSize(500,440);
    setWindowTitle("Super-Users For ACsystem");
    ui->hfee_edit->setText("1.0");
    ui->mfee_edit->setText("1.0");
    ui->lfee_edit->setText("1.0");
    ui->openroom_btn->setToolTip("开房");
    ui->report_btn->setToolTip("生成报表");
    ui->control_btn->setToolTip("更改空调参数");
    ui->powerOn_btn->setToolTip("空调开机");
    ui->start_supervise->setToolTip("开始监视");
    ui->maxtemp_edit->setReadOnly(true);
    ui->mintemp_edit->setReadOnly(true);
    supervise_timer = new QTimer(this);
    connect(supervise_timer, SIGNAL(timeout()), this, SLOT(queryRoomstate()));
    ui->powerOn_btn->setDisabled(true);
    ui->daily_rbtn->setChecked(true);
    Iniconnect();

}


void Widget::dealError(QJsonObject json){

}
//收到服务器确认包 则生成information提示
void Widget::dealConfirm(QJsonObject json)
{
    QString refId = json[REFID].toString();
    if(refIdlsit.contains(refId))
        refIdlsit.removeOne(json[REFID].toString());

    QMessageBox::information(this, "Message", "Complete!");
}



//对收到的包进行处理函数 所有的包在这里处理
void Widget::recvMsg(const QString& msg)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(msg.toUtf8(),&jsonError);
    if (!jsonDoc.isNull() && (jsonError.error == QJsonParseError::NoError))// 解析未发生错误
    {
        if (jsonDoc.isObject())// JSON 文档为对象
        {
            QJsonObject json = jsonDoc.object();
            if(refIdlsit.contains(json[REFID].toString()))
            {
                refIdlsit.removeOne(json[REFID].toString());
                 ui->rev_text->append(msg);
                 QJsonObject data = json[DATA].toObject();
                 QPoint widget_pos = mapToGlobal(QPoint(0,0));
                 QPoint t1;
                 QPoint t;
                 switch(sl.indexOf(json[HANDLER].toString())) {

                    case 0:// "/server/roomstate"
                     if(roomstate)
                     {
                         roomstate->close();
                         delete roomstate;
                         roomstate=nullptr;
                     }
                      //传入roomstate参数 生成状态监视状态信息
                    roomstate = new info(json,"roomState");
                    t1 = QCursor::pos();
                    t = mapFromGlobal(t1);
                    if(roomstate)
                    {
                        QSize size=roomstate->size();
                        int x=size.width();
                        int y=size.height();
                       roomstate->setGeometry(widget_pos.x()+this->frameGeometry().width(),widget_pos.y(),x,y);

                        roomstate->show();
                    }
                     break;
                    case 1: //   /server/report
                    if(report)
                    {
                        report->close();
                        delete report;
                        report=nullptr;
                    }
                    //传入report参数 生成报表信息
                    report = new info(data,"report");
                    t1 = QCursor::pos();
                    t = mapFromGlobal(t1);
                    if(report)
                    {
                        QSize size=report->size();
                        int x=size.width();
                        int y=size.height();
                       report->setGeometry(widget_pos.x()-x,widget_pos.y(),x,y);

                        report->show();
                    }
                    break;
                 case 2:
                 dealConfirm(json);

                };
                if(json[HANDLER]=="/server/error")
                {
                    if(json[MESSAGE]== "wrong password")
                        QMessageBox::warning(this, "wrong", "Wrong password!");
                }
                else if(json[HANDLER]=="/server/retRole")
                {


                }

            }

        }
    }

}
//初始化连接 连接信号与槽函数
void Widget::Iniconnect()
{
     client = new QWebSocket();
     connect(client, &QWebSocket::textMessageReceived, this,  &Widget::recvMsg);
     connectSrv();
}
//连接服务端
void Widget::connectSrv()
{
    QString path = QString("ws://%1:%2").arg(QString(SRVERADDR), QString(SRVERPORT));
    QUrl url = QUrl(path);

    client->open(url);
}
//发送报表查询包
void Widget::queryreport(int date1,int date2,QString type)
{
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = "/manager/queryreport";
    json[TOKEN] = token;
    QJsonObject data;
    data["type_Report"] = type;
    data["date1"] = date1;
    data["date2"] = date2;
    json[DATA] = data;
    auto jsonString = QString(QJsonDocument(json).toJson());
    client->sendTextMessage(jsonString);
    ui->rev_text->append(jsonString);
}
//发送开机请求包
void Widget::powerOn(QString mode,int tempHighLimit,int tempLowLimit,int defaultTargetTemp,double feeRateH,double feeRateM,double feeRateL)
{
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = "/manager/powerOn";
    json[TOKEN] = token;
    QJsonObject data;

    data[MODE] = mode;
    data[TEMPHLIMIT] = tempHighLimit;
    data[TEMPLLIMIT] = tempLowLimit;
    data[DEFAULTTARTEMP] = defaultTargetTemp;
    data[FEERATEH] = feeRateH;
    data[FEERATEM] = feeRateM;
    data[FEERATEL] = feeRateL;

    json[DATA] = data;
    auto jsonString = QString(QJsonDocument(json).toJson());
    client->sendTextMessage(jsonString);
     ui->rev_text->append(jsonString);
}
//发送更改费率报文
void Widget::changeFee(double feeRateH, double feeRateM, double feeRateL)
{
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = "/manager/powerOn";
    json[TOKEN] = token;
    QJsonObject data;
    data[FEERATEH] = 1;
    data[FEERATEM] = 1;
    data[FEERATEL] = 1;

    json[DATA] = data;
    auto jsonString = QString(QJsonDocument(json).toJson());
    client->sendTextMessage(jsonString);
     ui->rev_text->append(jsonString);
}


//发送开房请求报文
void Widget::openRoom(QString roomId)
{
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = "/manager/openRoom";
    QJsonObject data;
    data[ROOMID] = roomId;
    json[DATA] = data;
    auto jsonString = QString(QJsonDocument(json).toJson());
    client->sendTextMessage(jsonString);
     ui->rev_text->append(jsonString);
}
//发送房间状态查询请求包
void Widget::queryRoomstate()
{
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = "/manager/queryRoomState";
    json[TOKEN] = token;
    auto jsonString = QString(QJsonDocument(json).toJson());
     ui->rev_text->append(jsonString);
    client->sendTextMessage(jsonString);


}

Widget::~Widget()
{
    delete ui;
}
//设置默认的温度参数
void Widget::set_tartemp()
{
    ui->tartemp_sb->setMaximum(ui->maxtemp_edit->text().toUInt());
    ui->tartemp_sb->setMinimum(ui->mintemp_edit->text().toUInt());
    if(ui->cold_cbox->isChecked())
        ui->tartemp_sb->setValue(25);
    if(ui->heat_cbox->isChecked())
        ui->tartemp_sb->setValue(23);
}
//按下开房信号的槽函数 进行对应处理  设置图标状态，从图形化控件中获得信息 调用发送报文函数
void Widget::on_powerOn_btn_clicked()
{
    QString mode="";
    if(ui->cold_cbox->isChecked() && !ui->heat_cbox->isChecked())
        mode="cold";
    else if(!ui->cold_cbox->isChecked() && ui->heat_cbox->isChecked())
        mode="heat";
    double highfee=0.0,mfee=0.0,lfee=0.0;
    int htemp=0,ltemp=0,tartemp=0;
    highfee=ui->hfee_edit->text().toDouble();
    mfee=ui->mfee_edit->text().toDouble();
    lfee=ui->lfee_edit->text().toDouble();
    htemp=ui->maxtemp_edit->text().toUInt();
    ltemp=ui->mintemp_edit->text().toUInt();
    tartemp=ui->tartemp_sb->value();
    if(!isPower)//设置按钮图片 根据不同的状态设置
    {
        powerOn(mode,htemp,ltemp,tartemp,highfee,mfee,lfee);
        ui->powerOn_btn->setStyleSheet("QPushButton{border-image:url(:/off.png);}\
        QPushButton:hover{background-color:white; color: black;}QPushButton:pressed{ \
        border-style: inset; border-color: rgba(255, 225, 255, 30)}");
        isPower=true;
    }
    else
    {
        ui->powerOn_btn->setStyleSheet("QPushButton{border-image:url(:/on.png);}\
        QPushButton:hover{background-color:white; color: black;}QPushButton:pressed{ \
        border-style: inset; border-color: rgba(255, 225, 255, 30)}");
        isPower=false;
    }
    ui->manager_label->setStyleSheet("QLabel{border-image:url(:/manager_active.png)}");
//    powerOn("cold",24,16,24,2.0,1.0,0.5);
}

//按下生成报表的槽函数 进行对应处理  设置图标状态，从图形化控件中获得信息 调用发送报文函数
void Widget::on_report_btn_clicked()
{
    if(ui->starttime_edit->text()=="" || ui->endtime_edit->text()=="")
    {
        QMessageBox::warning(this, "wrong", "Please input start and end time!");
        return;
    }

    int start=ui->starttime_edit->text().toUInt();
    int end=ui->endtime_edit->text().toUInt();
    QString type="";
    if(ui->daily_rbtn->isChecked())type ="day";
    else if (ui->weekly_rbtn->isChecked()) type="week";
    queryreport(start,end,type);
}

//按下开始监视房间状态的槽函数 调用发送报文函数
void Widget::on_start_supervise_clicked()
{
    if(!isSupervise)
    {

         supervise_timer->start(10000);
         ui->start_supervise->setStyleSheet("QPushButton{border-image:url(:/supervise.png)}\
                                            QPushButton:hover{background-color:white; color: black;}");
         isSupervise=true;
         queryRoomstate();
    }
    else
    {
        supervise_timer->stop();
        isSupervise=false;
        ui->start_supervise->setStyleSheet("QPushButton{border-image:url(:/nosup.png)}\
                                           QPushButton:hover{background-color:white; color: black;}");
    }




}
//按下制冷模式的槽函数 进行对应处理  设置图标状态，从图形化控件中获得信息
void Widget::on_cold_cbox_stateChanged(int arg1)
{
    if(ui->cold_cbox->isChecked())
        ui->heat_cbox->setChecked(false);
    else
        ui->heat_cbox->setChecked(true);
    if(ui->cold_cbox->isChecked())
    {
        ui->maxtemp_edit->setText(QString::number(27));
        ui->mintemp_edit->setText(QString::number(16));
    }
    else
    {
        ui->maxtemp_edit->setText(QString::number(28));
        ui->mintemp_edit->setText(QString::number(22));
    }
    set_tartemp();
    if(ui->powerOn_btn->isEnabled()==false) ui->powerOn_btn->setEnabled(true);
}

//按下制热模式的槽函数 进行对应处理  设置图标状态，从图形化控件中获得信息
void Widget::on_heat_cbox_stateChanged(int arg1)
{

    if(ui->heat_cbox->isChecked())
        ui->cold_cbox->setChecked(false);
    else
        ui->cold_cbox->setChecked(true);
    if(ui->cold_cbox->isChecked())
    {
        ui->maxtemp_edit->setText(QString::number(27));
        ui->mintemp_edit->setText(QString::number(16));
    }
    else
    {
        ui->maxtemp_edit->setText(QString::number(28));
        ui->mintemp_edit->setText(QString::number(22));
    }
    set_tartemp();
    if(ui->powerOn_btn->isEnabled()==false) ui->powerOn_btn->setEnabled(true);
}
//按下控制中央空调的槽函数 进行对应处理  设置图标状态，从图形化控件中获得信息
void Widget::on_control_btn_clicked()
{
    changeFee(ui->hfee_edit->text().toDouble(),ui->mfee_edit->text().toDouble(),ui->lfee_edit->text().toDouble());
}

//按下图形控件逻辑函数
void Widget::on_weekly_rbtn_clicked()
{
    if(ui->weekly_rbtn->isChecked())
        ui->daily_rbtn->setChecked(false);
}

void Widget::on_daily_rbtn_clicked()
{
    if(ui->daily_rbtn->isChecked())
        ui->weekly_rbtn->setChecked(false);
}
//按下生成开放按钮对应槽函数 进行对应处理  设置图标状态，从图形化控件中获得信息 调用发包函数
void Widget::on_openroom_btn_clicked()
{
    QString roomId=ui->openroomid_edit->text();
    ui->openroomid_edit->clear();
    openRoom(roomId);
}
