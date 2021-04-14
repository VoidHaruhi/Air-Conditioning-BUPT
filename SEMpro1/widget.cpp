#include "widget.h"
#include "ui_widget.h"
Widget::Widget (QString token_local,QWidget *parent,int type)
    : QWidget(parent),token(token_local)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
//    client  = new QWebSocket();
    IniWidget();
//    token = token_local;
    Iniconnect();
    connectSrv();
//    getAllroom();




}
void Widget::IniWidget(){
    setFixedSize(1200,900);
    IniCtrlroom();
    IniOpenroom();
    IniCost();
}
void Widget::IniCloseroom()
{
//    ui->closeroom_tartemp_lEdit->clear();
    ui->closeroom_roomId_lEdit->clear();
    ui->closeroom_roomId_lEdit->setPlaceholderText("请输入房间号");
//    ui->closeroom_tartemp_lEdit->setPlaceholderText("请输入目标温度");
}
void Widget::IniOpenroom()
{
    ui->openroom_tartemp_lEdit->clear();
    ui->openroom_roomId_lEdit->clear();
    ui->openroom_roomId_lEdit->setPlaceholderText("请输入房间号");
    ui->openroom_tartemp_lEdit->setPlaceholderText("请输入目标温度");
}
void Widget::IniCtrlroom()
{
    ui->ctrlroom_wind_sb->setRange(0,3);
    ui->ctrlroom_wind_sb->setValue(1);
    ui->ctrlroom_tartemp_sb->setRange(16,30);
    ui->ctrlroom_tartemp_sb->setValue(24);

    ui->ctrlroom_wind_sb->setDisabled(true);
    ui->ctrlroom_tartemp_sb->setDisabled(true);
    ui->ctrlroom_poweron_rbtn->setDisabled(true);
    ui->ctrlroom_poweroff_rbtn->setDisabled(true);
    ui->ctrlroom_roomId_ledit->clear();
    ui->ctrlroom_roomId_ledit->setPlaceholderText("房间号默认为all");
    ui->checkBox->setChecked(false);
    ui->checkBox_2->setChecked(false);
    ui->checkBox_3->setChecked(false);

}
void Widget::dealRoomlist(QJsonObject json)
{
    QJsonArray list = json["roomList"].toArray();
    QJsonArray::Iterator iteratorJson;
    iteratorJson = list.begin();
    int cnt = 0;
    QStringList info = {};
     ui->Tablewidget->clear();
    while(iteratorJson != list.end()){
        QJsonObject room = iteratorJson->toObject();
        info<<room[ROOMID].toString()<<QString::number(room[ISIDLE].toBool());
//        qDebug()<<room[ROOMID]<<room[ISIDLE];
        cnt++;
        iteratorJson++;
    }
    int data_len = LEN_ROOMLIST;
    ui->Tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->Tablewidget->setRowCount(cnt);
    ui->Tablewidget->setColumnCount(data_len);
    ui->Tablewidget->setHorizontalHeaderLabels({"房间号", "空调状态"});
    ui->Tablewidget->verticalHeader()->hide();
   // ui->Tablewidget->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents);
    ui->Tablewidget->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch);
    QTableWidgetItem **t = new QTableWidgetItem *[cnt*data_len];

    int i;
   for(i = 0;i < cnt; i++)
   {
       t[data_len*i] = new QTableWidgetItem(info[data_len*i]);
        ui->Tablewidget->setItem(i, 0 , t[data_len*i]);
        //状态
        if(info[data_len*i+1]=="0") info[data_len*i+1] = "已登记";
        else info[data_len*i+1] = "idle";
       t[data_len*i+1] = new QTableWidgetItem(info[data_len*i+1]);
        ui->Tablewidget->setItem(i, 1 , t[data_len*i+1]);
    }
}
void Widget::dealError(QJsonObject json){

}
void Widget::dealConfirm(QJsonObject json)
{
    QString refId = json[REFID].toString();
    if(refIdlsit.contains(refId))
        refIdlsit.removeOne(json[REFID].toString());

    QMessageBox::information(this, "提示", "控制操作完成");
}
void Widget::dealdetailFeeList(QJsonObject json)
{
    QJsonArray list = json["costList"].toArray();
    QJsonArray::Iterator iteratorJson;
    iteratorJson = list.begin();
    int cnt = 0;
    QStringList info = {};
     ui->Tablewidget->clear();
    while(iteratorJson != list.end()){
        QJsonObject room = iteratorJson->toObject();
        info<<QString::number(room["time"].toInt())<<QString::number(room[NOWTEMP].toDouble())<<\
              QString::number(room[TARTEMP].toInt())<<QString::number(room[WIND].toInt())<<QString::number(room["fee"].toInt());
//        qDebug()<<room[ROOMID]<<room[ISIDLE];
        cnt++;
        iteratorJson++;
    }
    int data_len = LEN_ROOMINFO;
    int info_len = data_len-1;
    ui->Tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->Tablewidget->setRowCount(cnt);
    ui->Tablewidget->setColumnCount(data_len);
    ui->Tablewidget->setHorizontalHeaderLabels({"房间号", "运行时间", "当前温度","目标温度","当前风速","收费标准"});
    ui->Tablewidget->verticalHeader()->hide();
   // ui->Tablewidget->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents);
    ui->Tablewidget->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch);
    QTableWidgetItem **t = new QTableWidgetItem *[cnt*data_len];

    int i;
   for(i = 0;i < cnt; i++)
   {
       t[data_len*i] = new QTableWidgetItem(QString::number(json[ROOMID].toInt()));
        ui->Tablewidget->setItem(i, 0 , t[data_len*i]);

       t[data_len*i+1] = new QTableWidgetItem(info[info_len*i]);
       ui->Tablewidget->setItem(i, 1 , t[data_len*i+1]);

       t[data_len*i+2] = new QTableWidgetItem(info[info_len*i+1]);
       ui->Tablewidget->setItem(i, 2 , t[data_len*i+2]);

       t[data_len*i+3] = new QTableWidgetItem(info[info_len*i+2]);
       ui->Tablewidget->setItem(i, 3 , t[data_len*i+3]);

       t[data_len*i+4] = new QTableWidgetItem(info[info_len*i+3]);
        ui->Tablewidget->setItem(i, 4 , t[data_len*i+4]);

       t[data_len*i+5] = new QTableWidgetItem(info[info_len*i+4]);
         ui->Tablewidget->setItem(i, 5 , t[data_len*i+5]);
    }
}
void Widget::dealRoomInfo(QJsonObject json)
{
    QJsonArray list = json["roomInfoList"].toArray();
    QJsonArray::Iterator iteratorJson;
    iteratorJson = list.begin();
    int cnt = 0;
    QStringList info = {};
     ui->Tablewidget->clear();
    while(iteratorJson != list.end()){
        QJsonObject room = iteratorJson->toObject();
        info<<room[ROOMID].toString()<<QString::number(room[POWER].toInt())<<QString::number(room[TARTEMP].toInt())<<\
              QString::number(room[NOWTEMP].toInt())<<QString::number(room[WIND].toInt());
//        qDebug()<<room[ROOMID]<<room[ISIDLE];
        cnt++;
        iteratorJson++;
    }
    int data_len = LEN_ROOMINFO;
    ui->Tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->Tablewidget->setRowCount(cnt);
    ui->Tablewidget->setColumnCount(data_len);
    ui->Tablewidget->setHorizontalHeaderLabels({"房间号", "空调电源","目标温度","目标温度","当前风速"});
    ui->Tablewidget->verticalHeader()->hide();
   // ui->Tablewidget->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents);
    ui->Tablewidget->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch);
    QTableWidgetItem **t = new QTableWidgetItem *[cnt*data_len];

    int i;
   for(i = 0;i < cnt; i++)
   {
       t[data_len*i] = new QTableWidgetItem(info[data_len*i]);
        ui->Tablewidget->setItem(i, 0 , t[data_len*i]);
        //状态
//        if(info[data_len*i+1]=="false") info[data_len*i+1] = "关机";
//        else info[data_len*i+1] = "运行";

       t[data_len*i+1] = new QTableWidgetItem(info[data_len*i+1]);
       ui->Tablewidget->setItem(i, 1 , t[data_len*i+1]);

       t[data_len*i+2] = new QTableWidgetItem(info[data_len*i+2]);
       ui->Tablewidget->setItem(i, 2 , t[data_len*i+2]);

       t[data_len*i+3] = new QTableWidgetItem(info[data_len*i+3]);
       ui->Tablewidget->setItem(i, 3 , t[data_len*i+3]);

       t[data_len*i+4] = new QTableWidgetItem(info[data_len*i+4]);
        ui->Tablewidget->setItem(i, 4 , t[data_len*i+4]);
    }
}
void Widget::dealsimFee(QJsonObject json){
     QStringList info = {};
     info<<json[ROOMID].toString()<<QString::number(json["totalFee"].toDouble(), 'f', 1)<<QString::number(json["checkinTime"].toInt())<<\
           QString::number(json["checkoutTime"].toInt());
    int data_len = LEN_SIMCOST;
     ui->Tablewidget->clear();
    ui->Tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->Tablewidget->setRowCount(1);
    ui->Tablewidget->setColumnCount(data_len);
    ui->Tablewidget->setHorizontalHeaderLabels({"roomId", "totalFee", "checkinTime","checkoutTime"});
    ui->Tablewidget->verticalHeader()->hide();
   // ui->Tablewidget->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents);
    ui->Tablewidget->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch);
    QTableWidgetItem **t = new QTableWidgetItem *[1*data_len];

    int i;
   for(i = 0;i < 1; i++)
   {
       t[data_len*i] = new QTableWidgetItem(info[data_len*i]);
        ui->Tablewidget->setItem(i, 0 , t[data_len*i]);

       t[data_len*i+1] = new QTableWidgetItem(info[data_len*i+1]);
       ui->Tablewidget->setItem(i, 1 , t[data_len*i+1]);

       t[data_len*i+2] = new QTableWidgetItem(info[data_len*i+2]);
       ui->Tablewidget->setItem(i, 2 , t[data_len*i+2]);

       t[data_len*i+3] = new QTableWidgetItem(info[data_len*i+3]);
       ui->Tablewidget->setItem(i, 3 , t[data_len*i+3]);
    }
}
void Widget::dealReport(QJsonObject json){
    QJsonArray list = json["roomReportList"].toArray();
    QJsonArray::Iterator iteratorJson;
    iteratorJson = list.begin();
    int cnt = 0;
    QStringList info = {};
     ui->Tablewidget->clear();
    while(iteratorJson != list.end()){
        QJsonObject room = iteratorJson->toObject();
        info<<room[ROOMID].toString()<<QString::number(room["powerCost"].toDouble(),'f',1)<<QString::number(room["moneyCost"].toDouble(),'f',1);
//        qDebug()<<room[ROOMID]<<room[ISIDLE];
        cnt++;
        iteratorJson++;
    }
    int data_len = 3;
    ui->Tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->Tablewidget->setRowCount(cnt);
    ui->Tablewidget->setColumnCount(data_len);
    ui->Tablewidget->setHorizontalHeaderLabels({"roomId","powerCost","moneyCost"});
    ui->Tablewidget->verticalHeader()->hide();
   // ui->Tablewidget->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents);
    ui->Tablewidget->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch);
    QTableWidgetItem **t = new QTableWidgetItem *[cnt*data_len];

    int i;
   for(i = 0;i < cnt; i++)
   {
       t[data_len*i] = new QTableWidgetItem(info[data_len*i]);
        ui->Tablewidget->setItem(i, 0 , t[data_len*i]);
        //状态
//        if(info[data_len*i+1]=="false") info[data_len*i+1] = "关机";
//        else info[data_len*i+1] = "运行";

       t[data_len*i+1] = new QTableWidgetItem(info[data_len*i+1]);
       ui->Tablewidget->setItem(i, 1 , t[data_len*i+1]);

       t[data_len*i+2] = new QTableWidgetItem(info[data_len*i+2]);
       ui->Tablewidget->setItem(i, 2 , t[data_len*i+2]);

    }
}
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
                switch(sl.indexOf(json[HANDLER].toString())) {
                    case 0://   "/server/retRoomList" roomList[{roomId, idle}]

                    dealRoomlist(data);

                    break;
                    case 1://   "/server/retFeeList" beginTime,  endTime, totalcost<br>
                    //list[{time,roomId,currentTmp,targetTmp,fee}]
                    dealdetailFeeList(data);
                    break;
                    case 2://   "/server/retSimpleCost"6
                    dealsimFee(data);
                    break;
                    case 3://   "/server/roomInfo"
                    dealRoomInfo(data);
                    break;
                    case 4://   "/server/comfirm"
                    dealConfirm(json);
                    break;
                    case 5://   "/server/retReport"
                    dealReport(data);
                    break;

                    case 6://   "/server/error"

                    break;

                };
                if(json[HANDLER]=="/server/error")
                {
                    if(json[MESSAGE]== "wrong password")
                        QMessageBox::warning(this, "错误", "密码错误");
                }
                else if(json[HANDLER]=="/server/retRole")
                {


                }

            }

        }
    }

}
void Widget::Iniconnect()
{
//     connect(ui->login_btn, SIGNAL(clicked()), this, SLOT(Login_Pressed()));
//     connect(ui->signup_btn, SIGNAL(clicked()), this, SLOT(Signup_Pressed()));
     client = new QWebSocket();
//     connect(client, &QWebSocket::connected, this, &Widget::Connected);
     connect(client, &QWebSocket::textMessageReceived, this,  &Widget::recvMsg);
     connectSrv();
}
void Widget::getCost(QString roomId,bool detail,bool detailFIle)
{
    QString handler = "/manager/simpleCost";
    if(detail == true && detailFIle == true) handler = "/manager/detailCostFile";
    else if(detail == true)handler = "/manager/detailCost";
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = handler;
    json[TOKEN] = token;
    QJsonObject data;
    /*if(roomId!="All")data[ROOMID] = roomId.toInt();
    else*/
    data[ROOMID] = roomId;
    json[DATA] = data;
    auto jsonString = QString(QJsonDocument(json).toJson());
    client->sendTextMessage(jsonString);
}
void Widget::sendMSG(QString roomId,QString msg)
{
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = "/admin/sendMsg";
    QJsonObject data;
    data[MESSAGE] = msg;
    /*if(roomId!="all")data[ROOMID] = roomId.toInt();
    else*/ data[ROOMID] = roomId;
    json[DATA] = data;
    auto jsonString = QString(QJsonDocument(json).toJson());
    client->sendTextMessage(jsonString);
}

void Widget::getReport(){
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = "/manager/requestReport";
    json[TOKEN] = token;
    auto jsonString = QString(QJsonDocument(json).toJson());
    client->sendTextMessage(jsonString);
    ui->rev_text->append(jsonString);
}
void Widget::getAllroom()
{
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = "/manager/getRoomList";
    json[TOKEN] = token;
    auto jsonString = QString(QJsonDocument(json).toJson());
    client->sendTextMessage(jsonString);
    ui->rev_text->append(jsonString);
}
void Widget::controlRoom(QString roomId,QString temp,QString power,QString wind)
{
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = "/manager/controlRoom";
    json[TOKEN] = token;
    QJsonObject data;
    /*if(roomId!="all")data[ROOMID] = roomId.toInt();
    else*/ data[ROOMID] = roomId;
    data[TARTEMP] = temp.toDouble();

    /*if(power!="-1")data[POWER] = power;
    else*/ data[POWER] = power.toInt();
    data[WIND] = wind.toInt();
    json[DATA] = data;
    auto jsonString = QString(QJsonDocument(json).toJson());
    client->sendTextMessage(jsonString);
}


void Widget::closeRoom(QString roomId)
{
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = "/manager/closeRoom";
    json[TOKEN] = token;
    QJsonObject data;
    data[ROOMID] = roomId;
    json[DATA] = data;
    auto jsonString = QString(QJsonDocument(json).toJson());
    client->sendTextMessage(jsonString);
}
void Widget::openRoom(QString roomId,QString temp)
{
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = "/manager/openRoom";
    json[TOKEN] = token;
    QJsonObject data;
    data[ROOMID] = roomId/*.toInt()*/;
    data[DEFAULTMP] = temp.toDouble();
    json[DATA] = data;
    auto jsonString = QString(QJsonDocument(json).toJson());
    client->sendTextMessage(jsonString);
}
void Widget::seeRoomInfo(QString roomId)
{
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = "/manager/seeRoomInfo";
    json[TOKEN] = token;
    QJsonObject data;
    /*if(roomId!="All")data[ROOMID] = roomId.toInt();
    else*/ data[ROOMID] = roomId;
    json[DATA] = data;
    auto jsonString = QString(QJsonDocument(json).toJson());
    client->sendTextMessage(jsonString);
}
void Widget::connectSrv()
{
    QString path = QString("ws://%1:%2").arg(QString(SRVERADDR), QString(SRVERPORT));
    QUrl url = QUrl(path);

    client->open(url);
//    QTimer* my_time=new QTimer();
    connect(client,SIGNAL(connected()),this,SLOT(so_connected()));
}
Widget::~Widget()
{
    delete ui;
}


void Widget::on_allroomInfo_btn_clicked()
{
    getAllroom();
}




void Widget::on_roomInfo_btn_clicked()
{
    seeRoomInfo("All");
}

void Widget::on_checkBox_2_stateChanged(int arg1)
{
    if(arg1)ui->ctrlroom_tartemp_sb->setEnabled(true);
    else ui->ctrlroom_tartemp_sb->setDisabled(true);

//    if(ui->checkBox_2->isChecked())ui->ctrlroom_tartemp_sb->setEnabled(true);
}

void Widget::on_checkBox_stateChanged(int arg1)
{

    if(arg1)ui->ctrlroom_wind_sb->setEnabled(true);
    else ui->ctrlroom_wind_sb->setDisabled(true);
}

void Widget::on_checkBox_3_stateChanged(int arg1)
{

    if(arg1){
        ui->ctrlroom_poweron_rbtn->setEnabled(true);
        ui->ctrlroom_poweroff_rbtn->setEnabled(true);
    }
    else {
        ui->ctrlroom_poweron_rbtn->setDisabled(true);
        ui->ctrlroom_poweroff_rbtn->setDisabled(true);
    }
    ui->ctrlroom_poweroff_rbtn->setChecked(true);
}

void Widget::on_ctrlroom_btnbox_accepted()
{
    QString id = ui->ctrlroom_roomId_ledit->text();
    QString ttemp = QString::number(ui->ctrlroom_tartemp_sb->value());
    QString power = "";
    bool isPowerchanged = true;
    if(ui->checkBox_3->isChecked())
    {
        if(ui->ctrlroom_poweron_rbtn->isChecked())power = "1";
        else if(ui->ctrlroom_poweroff_rbtn->isChecked())power = "0";
    }
    else isPowerchanged = false;
    QString wind = QString::number(ui->ctrlroom_wind_sb->value());
    if(id =="")id = "all";
    if(isPowerchanged)controlRoom(id,ttemp,power,wind);
    else controlRoom(id,ttemp,"-1",wind);
     IniCtrlroom();
}

void Widget::on_ctrlroom_btnbox_rejected()
{
    IniCtrlroom();
}

void Widget::on_openroom_btnbox_accepted()
{
    QString id = ui->openroom_roomId_lEdit->text();
    QString temp = ui->openroom_tartemp_lEdit->text();
//    QRegExp rx("^\\d\\d$");

//    if(rx.indexIn(temp)!=0)
//    {
//        QMessageBox::information(this, "提示", "请输入正确的温度");
//        IniOpenroom();
//        return;
//    }
//    QRegExp rx1("^\\d\\d*$");
//    if(rx1.indexIn(id)!=0)
//    {
//        QMessageBox::information(this, "提示", "请输入正确的房间号");
//        IniOpenroom();
//        return;
//    }
    if(!id.isEmpty() && !temp.isEmpty())
        openRoom(id,temp);
    else{
        QMessageBox::information(this, "提示", "请输入房间号与温度");
    }
    IniOpenroom();
}
void Widget::IniCost()
{
    ui->cost_roomId_lEdit->setPlaceholderText("查询费用房间号");
    ui->cost_roomId_lEdit->clear();
}
void Widget::on_openroom_btnbox_rejected()
{
    IniOpenroom();
}

void Widget::on_Cost_clicked()
{
    getCost();
}
void Widget::so_connected()
{
    getAllroom();
    disconnect(client,SIGNAL(connected()),this,SLOT(so_connected()));

}

void Widget::on_simCost_btn_clicked()
{
    getCost();
    IniCost();
}

void Widget::on_detailCost_btn_clicked()
{
    getCost("All",true);
    IniCost();
}

void Widget::on_report_btn_clicked()
{
    getReport();
    IniCost();
}

void Widget::on_closeroom_btnbox_accepted()
{
    QString id = ui->closeroom_roomId_lEdit->text();
//    QString temp = ui->closeroom_tartemp_lEdit->text();
//    QRegExp rx("^\\d\\d$");

//    if(rx.indexIn(temp)!=0)
//    {
//        QMessageBox::information(this, "提示", "请输入正确的温度");
//        IniOpenroom();
//        return;
//    }
//    QRegExp rx1("^\\d\\d*$");
//    if(rx1.indexIn(id)!=0)
//    {
//        QMessageBox::information(this, "提示", "请输入正确的房间号");
//        IniOpenroom();
//        return;
//    }
    if(!id.isEmpty())
        closeRoom(id);
    else{
        QMessageBox::information(this, "提示", "请输入房间号与温度");
    }
    IniCloseroom();
}

void Widget::on_closeroom_btnbox_rejected()
{
    IniCloseroom();
}
