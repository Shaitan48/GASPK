#ifndef AGENT_H
#define AGENT_H

#include <QObject>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDir>

class Agent : public QObject
{
    Q_OBJECT

public:
    explicit Agent(QObject *parent = nullptr);

private slots:
    void sendSystemInfo();
    void onReply(QNetworkReply *reply);

public:
    QTimer *timer;
    QNetworkAccessManager *manager;
    QString serverUrl;
};

#endif // AGENT_H
