#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QList>
#include <QString>
#include <QSqlDatabase>
#include "Trigger.h"
struct AuthAgent {
    QString username;
    QString hostname;

    bool operator==(const AuthAgent& other) const {
        return username == other.username && hostname == other.hostname;
    }
};
class QTcpSocket;
class QNetworkReply;
class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    ~Server() override;

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
    QList<AuthAgent> allowedAgents;
    QSqlDatabase db;
    QString dbHost;
    int dbPort;
    QString dbUsername;
    QString dbPassword;
    QString dbName;
    QJsonObject config;
};

#endif // SERVER_H
