#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QNetworkAccessManager>
#include "MainWindow.h"

class QTcpSocket;
class QNetworkReply;

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
    void onReply(QNetworkReply* reply);


private:
    void sendMessage(const QString &message);
    void connectToServer();
    void sendSystemInfo();
    void loadOperations();
    void performOperation(const QJsonObject &operation);

    QTcpSocket* socket;
    QNetworkAccessManager* manager;
    MainWindow* mainWindow;


    bool authorized = false;
};

#endif // CLIENT_H
