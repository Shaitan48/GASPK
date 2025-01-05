#ifndef AGENT_H
#define AGENT_H

#include <QObject>
#include <QTcpSocket>
#include <QNetworkAccessManager>
#include <QList>
#include "Trigger.h"
class QTcpSocket;
class QNetworkReply;

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
    void onTriggered(const QJsonObject &result);

private:
    void sendMessage(const QString &message);
    void connectToServer();
    void sendSystemInfo();
    void loadTriggers();
    void loadOperations();
    void sendOperationResult(const QJsonObject& operationResult);
    void performOperation(const QJsonObject &operation);
    QTcpSocket* socket;
    QNetworkAccessManager* manager;
    QList<Trigger*> triggers;
};

#endif // AGENT_H
