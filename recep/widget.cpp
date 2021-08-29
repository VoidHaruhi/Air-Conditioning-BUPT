#include "widget.h"
#include "ui_widget.h"

widget::widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget)
{
    ui->setupUi(this);
    init_widget();
    init_connect();
}

widget::~widget()
{
    delete ui;
}

void widget::init_widget(){
    setWindowTitle("前台");
    setFixedSize(570,620);
    //设置窗口图标
    setWindowIcon(QIcon(":/image/title.jpg"));

}
void widget::paintEvent(QPaintEvent *){

    QPainter painter(this);
    QPixmap pix;
    pix.load(":/image/background1.jpg");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);



}

void widget::init_connect(){
    connect(ui->pushButton_1,&QPushButton::clicked,this,&widget::get_Invoice);
    connect(ui->pushButton_2,&QPushButton::clicked,this,&widget::print_Invoice);
    connect(ui->pushButton_3,&QPushButton::clicked,this,&widget::get_specification);
    connect(ui->pushButton_4,&QPushButton::clicked,this,&widget::print_specification);
    connect(reception,&QWebSocket::textMessageReceived,this,&widget::analyze_msg);

}
void widget::get_Invoice(){
    QString roomId = ui->lineEdit->text();
    QJsonObject json;
    QString refId = generate_refId();
    json.insert("refId",refId);
    json.insert("token",token);
    json.insert("handler","/reception/simpleCost");
    QJsonObject data;
    data.insert("roomId",roomId);
    json.insert("data",data);
    auto jsonString = QString(QJsonDocument(json).toJson());
    reception->sendTextMessage(jsonString);
}

void widget::get_specification(){
    QString roomId = ui->lineEdit->text();
    QJsonObject json;
    QString refId = generate_refId();
    json.insert("refId",refId);
    json.insert("token",token);
    json.insert("handler","/reception/detailCost");
    QJsonObject data;
    data.insert("roomId",roomId);
    json.insert("data",data);
    auto jsonString = QString(QJsonDocument(json).toJson());
    reception->sendTextMessage(jsonString);
}
void widget::analyze_msg(const QString& msg){
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(msg.toLocal8Bit().data(),&jsonError);
    if (!jsonDoc.isNull() && (jsonError.error == QJsonParseError::NoError))
    {  // 解析未发生错误
        if (jsonDoc.isObject())
        { // JSON 文档为对象
            QJsonObject json = jsonDoc.object();
            qDebug()<<json;
            if(refIdlist.contains(json["refId"].toString()))
            {
                refIdlist.removeOne(json["refId"].toString());
                if(json["handler"]=="/server/retSimpleCost"){
                    QJsonObject data  = json["data"].toObject();
                    qDebug()<<data;

                    QString roomId = data["roomId"].toString();
                    double fee = data["fee"].toDouble();
                    qDebug()<<roomId<<":    "<<fee<<endl;
                    show_Invoice(roomId,QString::asprintf("%.2f", fee));

                }else if(json["handler"] == "/server/retDetailCost"){
                    QJsonObject data = json["data"].toObject();
                    show_specification(data);

                }

            }

        }
    }
}
void widget::show_Invoice(QString roomId,QString fee){
    ui->textEdit->clear();
    ui->textEdit->insertPlainText("房间号\t"+roomId+'\n');
    ui->textEdit->insertPlainText("费用\t"+fee);
}
void widget::show_specification(QJsonObject json){
    ui->textEdit->clear();
    QJsonArray list = json["roomList"].toArray();
    QJsonArray::iterator it;
    it = list.begin();
    QJsonObject specification = it->toObject();
    QString roomId = specification["roomId"].toString();
    ui->textEdit->insertPlainText("房间号\t" + roomId + '\n');
    ui->textEdit->insertPlainText("风速\t费用\t费率\t请求修改时间\t持续时间\n");
    while (it != list.end()) {
        QJsonObject specification = it->toObject();
        int fanSpeed = specification["fanSpeed"].toInt();
        double fee = specification["fee"].toDouble();
        double feeRate = specification["feeRate"].toDouble();
        int requestDuration = specification["requestDuration"].toInt();
        int requestTime = specification["requestTime"].toInt();
        int hours,minutes,seconds;
        hours = requestTime/3600;
        minutes = requestTime/60 - hours*60;
        seconds = requestTime - minutes*60 - hours*3600;
        ui->textEdit->insertPlainText(QString::asprintf("%d", fanSpeed) + '\t');
        ui->textEdit->insertPlainText(QString::asprintf("%.2f", fee) + '\t');
        ui->textEdit->insertPlainText(QString::asprintf("%.2f", feeRate) + '\t');
        ui->textEdit->insertPlainText(QString::number(hours) + " 时 " +
                                      QString::number(minutes) + " 分 " + QString::number(seconds) + " 秒 \t");
        ui->textEdit->insertPlainText(QString::number(requestDuration) + '\n');

        it++;
    }
}
void widget::print_Invoice(){
    QString roomId = ui->lineEdit->text();
    QString filename = roomId + "_Invoice.csv";
    qDebug()<<filename;
    QFile file(filename);//文件命名
//    QFile file("D:/qtproject/reception/output/"+filename);//文件命名
    if (!file.open(QFile::WriteOnly | QFile::Text))		//检测文件是否打开
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QTextStream out(&file);					//分行写入文件
    QStringList list = ui->textEdit->toPlainText().split("\n");
    for(int i =0 ; i< list.size() ; i++){
        QString item = list[i].replace('\t',',');
        out << item << ",\n";
    }
    QMessageBox::information(this, "提示", "账单打印成功！");
}
void widget::print_specification(){
    QString roomId = ui->lineEdit->text();
    QString filename = roomId + "_specification.csv";
    qDebug()<<filename;
    QFile file(filename);//文件命名
//    QFile file("D:/qtproject/reception/output/"+filename);//文件命名
    if (!file.open(QFile::WriteOnly | QFile::Text))		//检测文件是否打开
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QTextStream out(&file);					//分行写入文件
    QStringList list = ui->textEdit->toPlainText().split("\n");
    for(int i =0 ; i< list.size() ; i++){
        QString item = list[i].replace('\t',',');
        out << item << ",\n";
    }
    QMessageBox::information(this, "提示", "详单打印成功！");


}
