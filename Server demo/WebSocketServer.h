#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>


namespace Ui {
    class WebSocketServer;
}

//服务端
class WebSocketServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit WebSocketServer(QWidget *parent = nullptr);
    ~WebSocketServer();

signals:
    void sendMessage(const QString &text);

private:
    // 清除所有客户端连接
    void clearClient();
    // 启动程序自动监听
    void autoListening();
    // 监听新连接
    void onNewConnection();
    // 进行包解析同时回复消息，这里只做了 {"request":1} 的测试
    void packageAnalyse(QWebSocket *socket, QJsonObject recvJson);

private:
    Ui::WebSocketServer *ui;

    QWebSocketServer *server;
    QList<QWebSocket*> clientList;
};


#endif // SERVER_H
