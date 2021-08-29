/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QPushButton *PowerButton;
    QPushButton *addTmp_Button;
    QPushButton *reduceTmp_Button;
    QLabel *targetTmp_valuelabel;
    QLabel *targetTmp_label;
    QLabel *Unit_label_2;
    QLabel *realTmp_valuelabel;
    QLabel *Unit_label_1;
    QLabel *realTmp_label;
    QLabel *wind_label;
    QPushButton *windClose_Button;
    QPushButton *lowSpeedButton;
    QPushButton *middleSpeed_Button;
    QPushButton *highSpeed_Button;
    QLabel *fee_label;
    QLabel *fee_valuelabel;
    QLabel *Unit_label_3;
    QLabel *work_label1;
    QLabel *work_label;
    QLabel *wind_label_2;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *time_label;
    QLabel *currentfee_label;
    QLabel *label_6;
    QLabel *label_7;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QStringLiteral("Widget"));
        Widget->resize(688, 557);
        Widget->setStyleSheet(QStringLiteral(""));
        PowerButton = new QPushButton(Widget);
        PowerButton->setObjectName(QStringLiteral("PowerButton"));
        PowerButton->setGeometry(QRect(510, 290, 51, 41));
        PowerButton->setStyleSheet(QLatin1String("				   QPushButton{\n"
"	\n"
"				   border-radius:10px;\n"
"				   \n"
"                   \n"
"	\n"
"	border-image: url(:/pictures/po1.png);\n"
"	\n"
"                   }\n"
"\n"
"                   QPushButton:checked{\n"
"                   \n"
"	border-image: url(:/pictures/po3.png);\n"
"	\n"
"	\n"
"				\n"
"	\n"
"	\n"
"                   }\n"
"                   \n"
"                   QPushButton:hover{\n"
"		\n"
"	background-color: rgb(0, 85, 255,0);\n"
"\n"
"                   }\n"
"\n"
""));
        PowerButton->setCheckable(true);
        PowerButton->setChecked(false);
        PowerButton->setAutoRepeat(true);
        PowerButton->setAutoExclusive(true);
        addTmp_Button = new QPushButton(Widget);
        addTmp_Button->setObjectName(QStringLiteral("addTmp_Button"));
        addTmp_Button->setGeometry(QRect(250, 400, 60, 60));
        addTmp_Button->setStyleSheet(QLatin1String("				   QPushButton{\n"
"				   border-radius:10px;\n"
"	\n"
"	border-image: url(:/pictures/add.png);\n"
"                                                                \n"
"                   }\n"
"\n"
"                   QPushButton:pressed{\n"
"                   \n"
"                   }\n"
"                   \n"
"                   QPushButton:hover{\n"
"background-color: rgb(0, 85, 255);\n"
"                   \n"
"	border-image: url(:/pictures/add2.png);\n"
"                   }"));
        reduceTmp_Button = new QPushButton(Widget);
        reduceTmp_Button->setObjectName(QStringLiteral("reduceTmp_Button"));
        reduceTmp_Button->setGeometry(QRect(250, 460, 60, 60));
        reduceTmp_Button->setStyleSheet(QLatin1String("				   QPushButton{\n"
"				   border-radius:10px;\n"
"	\n"
"	\n"
"	border-image: url(:/pictures/reduce.png);\n"
"                                                                \n"
"                   }\n"
"\n"
"                   QPushButton:pressed{\n"
"                   \n"
"                   }\n"
"                   \n"
"                   QPushButton:hover{\n"
"                   \n"
"	\n"
"	background-color: rgb(0, 85, 255);\n"
"	border-image: url(:/pictures/reduce2.png);\n"
"                   }"));
        targetTmp_valuelabel = new QLabel(Widget);
        targetTmp_valuelabel->setObjectName(QStringLiteral("targetTmp_valuelabel"));
        targetTmp_valuelabel->setGeometry(QRect(390, 50, 161, 101));
        targetTmp_valuelabel->setStyleSheet(QLatin1String("font: 48pt \"Agency FB\";\n"
"color: rgb(255, 255, 255);\n"
""));
        targetTmp_label = new QLabel(Widget);
        targetTmp_label->setObjectName(QStringLiteral("targetTmp_label"));
        targetTmp_label->setGeometry(QRect(300, 110, 81, 31));
        targetTmp_label->setStyleSheet(QString::fromUtf8("\n"
"font: 10pt \"\346\245\267\344\275\223\";\n"
"color: rgb(247,255,0);"));
        Unit_label_2 = new QLabel(Widget);
        Unit_label_2->setObjectName(QStringLiteral("Unit_label_2"));
        Unit_label_2->setGeometry(QRect(550, 80, 41, 61));
        Unit_label_2->setStyleSheet(QLatin1String("font: 26pt \"Agency FB\";\n"
"color: rgb(255, 255, 255);"));
        realTmp_valuelabel = new QLabel(Widget);
        realTmp_valuelabel->setObjectName(QStringLiteral("realTmp_valuelabel"));
        realTmp_valuelabel->setGeometry(QRect(90, 50, 161, 111));
        realTmp_valuelabel->setStyleSheet(QLatin1String("font: 48pt \"Agency FB\";\n"
"color: rgb(255, 255, 255);"));
        Unit_label_1 = new QLabel(Widget);
        Unit_label_1->setObjectName(QStringLiteral("Unit_label_1"));
        Unit_label_1->setGeometry(QRect(250, 90, 51, 51));
        Unit_label_1->setStyleSheet(QLatin1String("font: 26pt \"Agency FB\";\n"
"color: rgb(255, 255, 255);"));
        realTmp_label = new QLabel(Widget);
        realTmp_label->setObjectName(QStringLiteral("realTmp_label"));
        realTmp_label->setGeometry(QRect(10, 110, 81, 31));
        realTmp_label->setStyleSheet(QString::fromUtf8("color: rgb(247,255,0);\n"
"font: 10pt \"\346\245\267\344\275\223\";"));
        wind_label = new QLabel(Widget);
        wind_label->setObjectName(QStringLiteral("wind_label"));
        wind_label->setGeometry(QRect(10, 370, 71, 20));
        wind_label->setStyleSheet(QString::fromUtf8("color: rgb(0, 170, 255);\n"
"font: 9pt \"\346\245\267\344\275\223\";"));
        windClose_Button = new QPushButton(Widget);
        windClose_Button->setObjectName(QStringLiteral("windClose_Button"));
        windClose_Button->setGeometry(QRect(80, 350, 51, 41));
        windClose_Button->setStyleSheet(QLatin1String("				   QPushButton{\n"
"	color: rgb(255, 255, 255);\n"
"				   border-radius:10px;\n"
"				   background-color: rgb(85, 170, 255,100);\n"
"                                                                \n"
"                   }\n"
"\n"
"                   QPushButton:pressed{\n"
"                   \n"
"	\n"
"                   }\n"
"                   \n"
"                   QPushButton:hover{\n"
"                  background-color: rgb(0, 85, 255);\n"
"                   }\n"
"\n"
""));
        lowSpeedButton = new QPushButton(Widget);
        lowSpeedButton->setObjectName(QStringLiteral("lowSpeedButton"));
        lowSpeedButton->setGeometry(QRect(150, 350, 51, 41));
        lowSpeedButton->setStyleSheet(QLatin1String("				   QPushButton{\n"
"	color: rgb(255, 255, 255);\n"
"				   border-radius:10px;\n"
"				   background-color: rgb(85, 170, 255,100);\n"
"                                                                \n"
"                   }\n"
"\n"
"                   QPushButton:pressed{\n"
"                   \n"
"	\n"
"                   }\n"
"                   \n"
"                   QPushButton:hover{\n"
"                  background-color: rgb(0, 85, 255);\n"
"                   }\n"
"\n"
""));
        middleSpeed_Button = new QPushButton(Widget);
        middleSpeed_Button->setObjectName(QStringLiteral("middleSpeed_Button"));
        middleSpeed_Button->setGeometry(QRect(220, 350, 51, 41));
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(85, 170, 255, 100));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        QBrush brush2(QColor(255, 255, 255, 128));
        brush2.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Active, QPalette::PlaceholderText, brush2);
#endif
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        QBrush brush3(QColor(255, 255, 255, 128));
        brush3.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Inactive, QPalette::PlaceholderText, brush3);
#endif
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        QBrush brush4(QColor(255, 255, 255, 128));
        brush4.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Disabled, QPalette::PlaceholderText, brush4);
#endif
        middleSpeed_Button->setPalette(palette);
        middleSpeed_Button->setStyleSheet(QLatin1String("				   QPushButton{\n"
"	color: rgb(255, 255, 255);\n"
"				   border-radius:10px;\n"
"				   background-color: rgb(85, 170, 255,100);\n"
"                                                                \n"
"                   }\n"
"\n"
"                   QPushButton:pressed{\n"
"                   \n"
"	\n"
"                   }\n"
"                   \n"
"                   QPushButton:hover{\n"
"                  background-color: rgb(0, 85, 255);\n"
"                   }\n"
"\n"
""));
        highSpeed_Button = new QPushButton(Widget);
        highSpeed_Button->setObjectName(QStringLiteral("highSpeed_Button"));
        highSpeed_Button->setGeometry(QRect(290, 350, 51, 41));
        highSpeed_Button->setStyleSheet(QLatin1String("				   QPushButton{\n"
"	color: rgb(255, 255, 255);\n"
"				   border-radius:10px;\n"
"				   background-color: rgb(85, 170, 255,100);\n"
"                                                                \n"
"                   }\n"
"\n"
"                   QPushButton:pressed{\n"
"                   \n"
"	\n"
"                   }\n"
"                   \n"
"                   QPushButton:hover{\n"
"                  background-color: rgb(0, 85, 255);\n"
"                   }\n"
"\n"
""));
        fee_label = new QLabel(Widget);
        fee_label->setObjectName(QStringLiteral("fee_label"));
        fee_label->setGeometry(QRect(10, 260, 72, 15));
        fee_label->setStyleSheet(QString::fromUtf8("\n"
"font: 10pt \"\346\245\267\344\275\223\";\n"
"color: rgb(247,255,0);"));
        fee_valuelabel = new QLabel(Widget);
        fee_valuelabel->setObjectName(QStringLiteral("fee_valuelabel"));
        fee_valuelabel->setGeometry(QRect(80, 180, 161, 121));
        fee_valuelabel->setStyleSheet(QLatin1String("font: 48pt \"Agency FB\";\n"
"color: rgb(255, 255, 255);"));
        Unit_label_3 = new QLabel(Widget);
        Unit_label_3->setObjectName(QStringLiteral("Unit_label_3"));
        Unit_label_3->setGeometry(QRect(240, 260, 91, 16));
        Unit_label_3->setStyleSheet(QString::fromUtf8("color: rgb(0, 170, 255);\n"
"font: 9pt \"\346\245\267\344\275\223\";"));
        work_label1 = new QLabel(Widget);
        work_label1->setObjectName(QStringLiteral("work_label1"));
        work_label1->setGeometry(QRect(70, 530, 72, 15));
        work_label1->setStyleSheet(QString::fromUtf8("color: rgb(0, 170, 255);\n"
"font: 9pt \"\346\245\267\344\275\223\";"));
        work_label = new QLabel(Widget);
        work_label->setObjectName(QStringLiteral("work_label"));
        work_label->setGeometry(QRect(70, 410, 60, 60));
        work_label->setStyleSheet(QLatin1String("border-radius:10px;\n"
"\n"
"\n"
""));
        wind_label_2 = new QLabel(Widget);
        wind_label_2->setObjectName(QStringLiteral("wind_label_2"));
        wind_label_2->setGeometry(QRect(340, 170, 201, 101));
        label = new QLabel(Widget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(90, 480, 72, 15));
        label->setStyleSheet(QString::fromUtf8("color: rgb(0, 170, 255);\n"
"font: 9pt \"\346\245\267\344\275\223\";"));
        label_2 = new QLabel(Widget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 140, 521, 16));
        label_2->setStyleSheet(QStringLiteral(""));
        label_3 = new QLabel(Widget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 280, 511, 16));
        label_3->setStyleSheet(QStringLiteral(""));
        label_4 = new QLabel(Widget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(250, 530, 72, 15));
        label_4->setStyleSheet(QString::fromUtf8("color: rgb(0, 170, 255);\n"
"font: 9pt \"\346\245\267\344\275\223\";"));
        time_label = new QLabel(Widget);
        time_label->setObjectName(QStringLiteral("time_label"));
        time_label->setGeometry(QRect(10, 10, 341, 16));
        time_label->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 255);\n"
"font: 9pt \"\346\245\267\344\275\223\";"));
        currentfee_label = new QLabel(Widget);
        currentfee_label->setObjectName(QStringLiteral("currentfee_label"));
        currentfee_label->setGeometry(QRect(180, 160, 61, 31));
        currentfee_label->setStyleSheet(QLatin1String("font: 18pt \"Agency FB\";\n"
"color: rgb(255, 255, 255);"));
        label_6 = new QLabel(Widget);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(240, 170, 68, 15));
        label_6->setStyleSheet(QString::fromUtf8("color: rgb(0, 170, 255);\n"
"font: 9pt \"\346\245\267\344\275\223\";"));
        label_7 = new QLabel(Widget);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(50, 170, 68, 15));
        label_7->setStyleSheet(QString::fromUtf8("\n"
"font: 10pt \"\346\245\267\344\275\223\";\n"
"color: rgb(247,255,0);"));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "Widget", Q_NULLPTR));
        PowerButton->setText(QString());
        addTmp_Button->setText(QString());
        reduceTmp_Button->setText(QString());
        targetTmp_valuelabel->setText(QApplication::translate("Widget", "25", Q_NULLPTR));
        targetTmp_label->setText(QApplication::translate("Widget", "\347\233\256\346\240\207\346\270\251\345\272\246", Q_NULLPTR));
        Unit_label_2->setText(QApplication::translate("Widget", "\342\204\203", Q_NULLPTR));
        realTmp_valuelabel->setText(QApplication::translate("Widget", "25.0", Q_NULLPTR));
        Unit_label_1->setText(QApplication::translate("Widget", "\342\204\203", Q_NULLPTR));
        realTmp_label->setText(QApplication::translate("Widget", "\345\256\236\351\231\205\346\270\251\345\272\246", Q_NULLPTR));
        wind_label->setText(QApplication::translate("Widget", "\351\243\216\351\200\237", Q_NULLPTR));
        windClose_Button->setText(QApplication::translate("Widget", "close", Q_NULLPTR));
        lowSpeedButton->setText(QApplication::translate("Widget", "\344\275\216", Q_NULLPTR));
        middleSpeed_Button->setText(QApplication::translate("Widget", "\344\270\255", Q_NULLPTR));
        highSpeed_Button->setText(QApplication::translate("Widget", "\351\253\230", Q_NULLPTR));
        fee_label->setText(QApplication::translate("Widget", "\350\264\271\347\224\250", Q_NULLPTR));
        fee_valuelabel->setText(QApplication::translate("Widget", "10.00", Q_NULLPTR));
        Unit_label_3->setText(QApplication::translate("Widget", "\345\205\203  \351\243\216\351\200\237\347\255\211\347\272\247", Q_NULLPTR));
        work_label1->setText(QApplication::translate("Widget", "\345\267\245\344\275\234\346\250\241\345\274\217", Q_NULLPTR));
        work_label->setText(QString());
        wind_label_2->setText(QString());
        label->setText(QString());
        label_2->setText(QApplication::translate("Widget", "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", Q_NULLPTR));
        label_3->setText(QApplication::translate("Widget", "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", Q_NULLPTR));
        label_4->setText(QApplication::translate("Widget", "\346\270\251\345\272\246\350\260\203\350\212\202", Q_NULLPTR));
        time_label->setText(QString());
        currentfee_label->setText(QApplication::translate("Widget", "2.0", Q_NULLPTR));
        label_6->setText(QApplication::translate("Widget", "\345\205\203", Q_NULLPTR));
        label_7->setText(QApplication::translate("Widget", "\345\275\223\345\211\215\350\264\271\347\224\250", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
