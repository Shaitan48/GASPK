// server.h
#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSqlDatabase>

class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

    bool startServer(quint16 port);
    void stopServer();

private:
    QSqlDatabase database;
    QNetworkAccessManager *networkManager;

    void sendRequest(const QString &url);

private slots:
    void onReplyFinished(QNetworkReply *reply);
};

#endif // SERVER_H
