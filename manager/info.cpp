#include "info.h"
#include "ui_info.h"


/**
* @projectName   SEMpro2
* @brief         两个报表的构造函数，动态构造 以及打印出对应的csv表格文件
* @author        Oliver
* @module        该部分模块由widget模块动态调用，在收到对应的信息包之后动态构造
* @date          2021-06-30
*/
info::info(QJsonObject json,QString type,QWidget *parent) ://收到的json报文，type:房间状态或者报表打印，parent 父类
    QScrollArea(parent),
    ui(new Ui::info)
{
    ui->setupUi(this);
     setWindowTitle(type);
     setFixedSize(450,600);
//    this->set
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏横向滚动条
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏竖向滚动条
//    this->setWindowFlags(windowFlags()|Qt::FramelessWindowHint);
    table = new QTableWidget(this);
    table->setShowGrid(false);//隐藏表格线
    //设置不可选中
    table->resize(450,600);
    table->clear();
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //    table->horizontalHeader()->hide();
    table->verticalHeader()->hide();
    // table->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch);

    QJsonArray list = json["data"].toArray();
    if(type=="report") list = json["reportList"].toArray();
    QJsonArray::Iterator iteratorJson;
    iteratorJson = list.begin();
    int cnt = 0;
    QStringList info = {};
    table->clear();


    int data_len=0;
    if(type=="report"){
        data_len=8;
        table->setColumnCount(data_len);
        table->setHorizontalHeaderLabels({"id", "调风数", "调度数","详单数","调温","调风","服务时间","总费用"});
        while(iteratorJson != list.end()){
            QJsonObject room = iteratorJson->toObject();
            info<<room[ROOMID].toString()<<QString::number(room["switchTimes"].toInt())<<QString::number(room["scheduleTimes"].toInt())<<\
                  QString::number(room["rdCount"].toInt())<<\
                 QString::number(room["tempChangeTimes"].toInt())<<QString::number(room["spdChangeTimes"].toInt())<<\
                  QString::number(room["requestTime"].toDouble(),'f',2)<<QString::number(room["totalFee"].toDouble(),'f',2);
    //        qDebug()<<room[ROOMID]<<room[ISIDLE];
            cnt++;
            iteratorJson++;
        }
    }
    else if(type=="roomState"){
        data_len=8;
        table->setColumnCount(data_len);
        table->setHorizontalHeaderLabels({"id", "运行", "室温","设温","风速","价格","总价","调度状态"});
        while(iteratorJson != list.end()){
            QJsonObject room = iteratorJson->toObject();
            info<<room[ROOMID].toString()<<room["isPowerOn"].toString()<<QString::number(room["currentTemp"].toDouble(),'f',2)<<\
                  QString::number(room["targetTemp"].toDouble(),'f',2)<<\
                 QString::number(room["fanSpeed"].toInt())<<QString::number(room["fee"].toDouble(),'f',2)<<\
                  QString::number(room["totalFee"].toDouble(),'f',2)<<room["state"].toString();
    //        qDebug()<<room[ROOMID]<<room[ISIDLE];
            cnt++;
            iteratorJson++;
        }
    }

    table->setRowCount(cnt);

    QTableWidgetItem **t = new QTableWidgetItem *[cnt*data_len];

    int i;
    for(i = 0;i < cnt; i++)
    {
        for(int j=0;j<data_len;j++)
        {
            t[data_len*i+j] = new QTableWidgetItem(info[data_len*i+j]);
             table->setItem(i, j , t[data_len*i+j]);
        }

    }
   table->setStyleSheet(
   "QTableView::item\
    {  \
        color:#677483;\
        gridline-color: #ccddf0;\
    }\
    QTableView::item:selected\
    { \
        color:#677483;\
    }\
     \
    QHeaderView::section { \
        color: black;\
        text-align:center;\
        height:32px;\
        background-color: #d1dff0;\
        border:1px solid #8faac9;\
        border-left:none;\
    }\
     \
    // border-left:none;防止中间表头的border重叠\
    QHeaderView::section:first\
    {\
        border-left:1px solid #8faac9;\
    }");

    this->setWidget(table);
    SaveCsv(type);


}



void info::SaveCsv(QString type)//打印出对应的csv文件
{
    QString filename=type+".csv";

    QFile file(filename);
    bool ret = file.open(QIODevice::Truncate | QIODevice::ReadWrite);
    if(!ret)
    {
        qDebug() << "open failure";
    }

    QTextStream stream(&file);
    QString conTents;
    // 写入头
    QHeaderView * header = table->horizontalHeader() ;
    if (NULL != header)
    {
        for (int i = 0; i < header->count(); i++)
        {
            QTableWidgetItem *item = table->horizontalHeaderItem(i);
            if (NULL != item)
            {
                conTents += item->text() + ",";
            }
        }
        conTents += "\n";
    }

    // 写内容
    for (int row = 0; row < table->rowCount(); row++)
    {
        for (int column = 0; column < table->columnCount(); column++)
        {
            QTableWidgetItem* item = table->item(row, column);
            if (NULL != item )
            {
                QString str;
                str = item->text();
                str.replace(","," ");
                conTents += str + ",";
            }
        }
        conTents += "\n";
    }
    stream << conTents;

    file.close();

}
info::~info()
{
    delete ui;
}
