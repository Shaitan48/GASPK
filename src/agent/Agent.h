#ifndef AGENT_H
#define AGENT_H

#include <QObject>
#include <QTcpSocket>
#include <QList>
#include "Task.h"

class QTcpSocket;
class QNetworkAccessManager;
class QNetworkReply;
class Task;

class Agent : public QObject
{
    Q_OBJECT

public:
    explicit Agent(QObject *parent = nullptr);
    ~Agent() override;

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onReply(QNetworkReply* reply);

private:
    void sendSystemInfo();
    void loadConfig();
    Task* findTaskById(qlonglong id);
    QTcpSocket* tcpSocket;
    QNetworkAccessManager * manager;
    QList<Task*> tasks;

};

#endif // AGENT_H
