#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QList>
#include "Task.h"
#include <QNetworkAccessManager>

class QTcpSocket;
class QNetworkReply;
class Task;

class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    ~Server() override;

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
    void onReply(QNetworkReply* reply);

private:
    void sendOperationsToClient(QTcpSocket* client);
    void sendSystemInfoToClient(QTcpSocket* client);
    void loadTasksFromFile();
    void saveTasksToFile() const;
    void handleTaskStateChanged(const QJsonObject & taskStateChange, QTcpSocket* client);
    void sendTriggersToAgent(QTcpSocket* agent);
    QTcpServer* tcpServer;
    QNetworkAccessManager * manager;
    QMap<QTcpSocket*, QString> clients;
    QList<Task*> tasks;
};

#endif // SERVER_H
