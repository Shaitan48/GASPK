#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QMap>
#include <QList>
#include "../common/Task.h"
#include "SqlDatabase.h"

class QTcpServer;
class QTcpSocket;
class QNetworkAccessManager;
class QNetworkReply;
class QJsonObject;

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
    void handleTaskStateChanged(const QJsonObject& taskStateChange, QTcpSocket* client);


private:
    void loadTasksFromFile();
    void saveTasksToFile() const;
    void sendTriggersToAgent(QTcpSocket* agent);
    void sendOperationsToClient(QTcpSocket* client);
    void sendAuthResult(QTcpSocket* client, bool authResult);
    void sendSystemInfoToClient(QTcpSocket* client);
    QTcpServer* tcpServer;
    QNetworkAccessManager * manager;
    QMap<QTcpSocket*, QString> clients;
    QList<Task*> tasks;
    QList<QJsonObject> allowedAgents;
    bool isAgentAllowed(const QString& username, const QString& hostname) const;
    SqlDatabase* db;

};

#endif // SERVER_H
