#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>

class Client : public QWidget
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);
    ~Client();

    void sendRequest(const QString &url);

private:
    QNetworkAccessManager *networkManager;

    // GUI элементы
    QVBoxLayout *layout;
    QLabel *urlLabel;
    QLineEdit *urlInput;
    QPushButton *sendButton;
    QLabel *responseLabel;

private slots:
    void onReplyFinished(QNetworkReply *reply);
};

#endif // CLIENT_H
