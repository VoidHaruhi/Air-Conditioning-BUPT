#ifndef WIGHT_H
#define WIGHT_H

#include <QWidget>

namespace Ui {
class wight;
}
class wight : public QWidget
{
    Q_OBJECT
public:
    explicit wight(QWidget *parent = nullptr);
    ~wight();
private:
    Ui::wight *ui;
};
#endif // WIGHT_H
