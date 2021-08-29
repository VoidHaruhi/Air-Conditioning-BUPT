/**
 * @title widget.h
 * @author 唐方柳
 */
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "head.h"

namespace Ui {
class widget;
}
/**
 * @brief The widget class 账单、详单的显示及打印
 * @author 唐方柳
 */
class widget : public QWidget
{
    Q_OBJECT

public:
    explicit widget(QWidget *parent = nullptr);
    ~widget();
    /**
     * @brief init_widget 初始化界面
     */
    void init_widget();
    /**
     * @brief get_Invoice 发送查询账单包
     */
    void get_Invoice();
    /**
     * @brief get_specification 发送查询详单包
     */
    void get_specification();
    /**
     * @brief analyze_msg 解析返回查询详单包
     * @param msg 数据包
     */
    void analyze_msg(const QString& msg);
    /**
     * @brief print_Invoice 打印账单
     */
    void print_Invoice();
    /**
     * @brief print_specification 打印详单
     */
    void print_specification();
    /**
     * @brief show_Invoice 显示账单
     * @param roomId 房间号
     * @param fee 费用
     */
    void show_Invoice(QString roomId,QString fee);
    /**
     * @brief show_specification  显示详单
     * @param josn josn格式的数据包
     */
    void show_specification(QJsonObject josn);
    /**
     * @brief init_connect 信号与槽连接
     */
    void init_connect();


private:
    Ui::widget *ui;
    /**
     * @brief paintEvent 画背景图
     */
    void paintEvent(QPaintEvent *);

};

#endif // WIDGET_H
