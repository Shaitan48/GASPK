#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QList>
#include "Task.h"
class QTcpSocket;
class Task;

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    ~Client() override;
private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void handleTaskStateChanged(const QJsonObject& taskStateChange, QTcpSocket* client);
private:
    QTcpSocket* tcpSocket;
    QList<Task*> tasks;
    void loadConfig();
};

#endif // CLIENT_H
