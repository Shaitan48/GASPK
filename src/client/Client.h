#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
class QTcpSocket;
class QNetworkReply;
class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    void sendMessage(const QString& message);
private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onReply(QNetworkReply* reply);

private:
    void sendSystemInfo();
    void connectToServer();
    QTcpSocket* socket;
    QNetworkAccessManager* manager;
};

#endif // CLIENT_H
