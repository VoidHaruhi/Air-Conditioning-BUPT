#include "WebSocketServer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WebSocketServer w;
    // 不显示ui, 直接在命令行显示消息
    //w.show();
    return a.exec();
}
