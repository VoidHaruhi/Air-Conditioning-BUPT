#include "wight.h"
#include "ui_wight.h"

wight::wight(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wight)
{
    ui->setupUi(this);
}

wight::~wight()
{
    delete ui;
}
