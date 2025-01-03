#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include "Task.h"

class QTcpSocket;
class QNetworkReply;

class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);

private slots:
    void onNewConnection();
    void handleClient(QTcpSocket *socket);
    void onReply(QNetworkReply* reply);

private:
    void loadConfig();
    QString getFormattedDateTime();
    QTcpServer* server;
    QNetworkAccessManager* manager;
    int serverPort;
};

#endif // SERVER_H
