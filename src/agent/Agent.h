#ifndef AGENT_H
#define AGENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>

    class Agent : public QObject
{
    Q_OBJECT

public:
    explicit Agent(QObject *parent = nullptr);
    ~Agent();

    void start();
    void stop();
    void sendRequest(const QString &url);

private:
    QNetworkAccessManager *networkManager;
    QSqlDatabase database;

    void loadConfiguration();

private slots:
    void onReplyFinished(QNetworkReply *reply);
};

#endif // AGENT_H
