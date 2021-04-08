#include "widget.h"
#include "ui_widget.h"
Widget::Widget(QString token_local,QWidget *parent,int type)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
//    client  = new QWebSocket();
    IniWidget();
    Iniconnect();
    getAllroom();
    token = token_local;
    connectSrv();


}
void Widget::IniWidget(){
    setFixedSize(1200,900);
    IniCtrlroom();
    IniOpenroom();
}
void Widget::IniOpenroom()
{
    ui->openroom_tartemp_lEdit->clear();
    ui->openroom_roomId_lEdit->clear();
    ui->openroom_roomId_lEdit->setPlaceholderText(tr("请输入房间号"));
    ui->openroom_tartemp_lEdit->setPlaceholderText(tr("请输入目标温度"));
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
    ui->ctrlroom_roomId_ledit->setPlaceholderText(tr("房间号默认为all"));
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
        info<<QString::number(room[ROOMID].toInt())<<room[ISIDLE].toString();
//        qDebug()<<room[ROOMID]<<room[ISIDLE];
        cnt++;
        iteratorJson++;
    }
    int data_len = LEN_ROOMLIST;
    ui->Tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->Tablewidget->setRowCount(cnt);
    ui->Tablewidget->setColumnCount(data_len);
    ui->Tablewidget->setHorizontalHeaderLabels({tr("房间号"), tr("空调状态")});
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
        if(info[data_len*i+1]=="false") info[data_len*i+1] = "关机";
        else info[data_len*i+1] = "运行";
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

    QMessageBox::information(this, tr("提示"), tr("控制操作完成"));
}
void Widget::dealFeeList(QJsonObject json)
{
    {
        QJsonArray list = json["retFeeList"].toArray();
        QJsonArray::Iterator iteratorJson;
        iteratorJson = list.begin();
        int cnt = 0;
        QStringList info = {};
         ui->Tablewidget->clear();
        while(iteratorJson != list.end()){
            QJsonObject room = iteratorJson->toObject();
            info<<room[ROOMID].toString()<<room[ISIDLE].toString();
    //        qDebug()<<room[ROOMID]<<room[ISIDLE];
            cnt++;
            iteratorJson++;
        }
        ui->Tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->Tablewidget->setRowCount(cnt);
        ui->Tablewidget->setColumnCount(2);
        ui->Tablewidget->setHorizontalHeaderLabels({tr("房间号"), tr("空调状态")});
        ui->Tablewidget->verticalHeader()->hide();
       // ui->Tablewidget->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents);
        ui->Tablewidget->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch);
        QTableWidgetItem **t = new QTableWidgetItem *[cnt*2];
        int data_len = 2;
        int i;
       for(i = 0;i < cnt; i++)
       {
           t[data_len*i] = new QTableWidgetItem(info[data_len*i]);
            ui->Tablewidget->setItem(i, 0 , t[2*i]);
            //状态
            if(info[data_len*i+1]=="false") info[data_len*i+1] = "关机";
            else info[data_len*i+1] = "运行";
           t[data_len*i+1] = new QTableWidgetItem(info[data_len*i+1]);
            ui->Tablewidget->setItem(i, 1 , t[2*i+1]);
        }
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
        info<<QString::number(room[ROOMID].toInt())<<room[POWER].toString()<<QString::number(room[TARTEMP].toInt())<<\
              QString::number(room[NOWTEMP].toInt())<<QString::number(room[WIND].toInt());
//        qDebug()<<room[ROOMID]<<room[ISIDLE];
        cnt++;
        iteratorJson++;
    }
    int data_len = LEN_ROOMINFO;
    ui->Tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->Tablewidget->setRowCount(cnt);
    ui->Tablewidget->setColumnCount(data_len);
    ui->Tablewidget->setHorizontalHeaderLabels({tr("房间号"), tr("空调电源"),tr("目标温度"),tr("目标温度"),tr("当前风速")});
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

                    break;
                    case 2://   "/server/retSimpleCost"6
                    case 3://   "/server/roomInfo"
                    dealRoomInfo(data);
                    case 4://   "/server/comfirm"

                    case 5://   "/server/error"

                    break;

                };
                if(json[HANDLER]=="/server/error")
                {
                    if(json[MESSAGE]== "wrong password")
                        QMessageBox::warning(this, tr("错误"), tr("密码错误"));
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
    QString handler = "/admin/simpleCost";
    if(detail == true && detailFIle == true) handler = "/admin/detailCostFile";
    else if(detail == true)handler = "/admin/detailCost";
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = handler;
    json[TOKEN] = token;
    QJsonObject data;
    if(roomId!="all")data[ROOMID] = roomId.toInt();
    else data[ROOMID] = roomId;
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
void Widget::getAllroom()
{
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = "/manager/retRoomList";
    json[TOKEN] = token;
    auto jsonString = QString(QJsonDocument(json).toJson());
    client->sendTextMessage(jsonString);
}
void Widget::controlRoom(QString roomId,QString temp,QString power,QString wind)
{
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = "/manager/controlRoom";
    QJsonObject data;
    if(roomId!="all")data[ROOMID] = roomId.toInt();
    else data[ROOMID] = roomId;
    data[TARTEMP] = temp.toInt();
    if(power!="-1")data[POWER] = power;
    else data[POWER] = power.toInt();
    data[WIND] = wind.toInt();
    json[DATA] = data;
    auto jsonString = QString(QJsonDocument(json).toJson());
    client->sendTextMessage(jsonString);
}

void Widget::openRoom(QString roomId,QString temp)
{
    QJsonObject json;
    json[REFID] = generate_refId();
    json[HANDLER] = "/manager/openRoom";
    QJsonObject data;
    data[ROOMID] = roomId.toInt();
    data[DEFAULTMP] = temp.toInt();
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
    if(roomId!="all")data[ROOMID] = roomId.toInt();
    else data[ROOMID] = roomId;
    json[DATA] = data;
    auto jsonString = QString(QJsonDocument(json).toJson());
    client->sendTextMessage(jsonString);
}
void Widget::connectSrv()
{
    QString path = QString("ws://%1:%2").arg(QString(SRVERADDR), QString(SRVERPORT));
    QUrl url = QUrl(path);

    client->open(url);
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
    seeRoomInfo("all");
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
        if(ui->ctrlroom_poweron_rbtn->isChecked())power = "on";
        else if(ui->ctrlroom_poweroff_rbtn->isChecked())power = "off";
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
    QRegExp rx("^\\d\\d$");

    if(rx.indexIn(temp)!=0)
    {
        QMessageBox::information(this, tr("提示"), tr("请输入正确的温度"));
        IniOpenroom();
        return;
    }
    QRegExp rx1("^\\d\\d?$");
    if(rx1.indexIn(id)!=0)
    {
        QMessageBox::information(this, tr("提示"), tr("请输入正确的房间号"));
        IniOpenroom();
        return;
    }
    if(!id.isEmpty() && !temp.isEmpty())
        openRoom(id,temp);
    else{
        QMessageBox::information(this, tr("提示"), tr("请输入房间号与温度"));
    }
    IniOpenroom();
}

void Widget::on_openroom_btnbox_rejected()
{
    IniOpenroom();
}

void Widget::on_Cost_clicked()
{
    getCost();
}
