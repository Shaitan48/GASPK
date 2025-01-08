#include "Client.h"
#include <QDebug>
#include <QNetworkRequest>
#include <QUrl>

Client::Client(QWidget *parent)
    : QWidget(parent),
    networkManager(new QNetworkAccessManager(this))
{
    // Создание интерфейса
    layout = new QVBoxLayout(this);
    urlLabel = new QLabel("Введите URL:", this);
    urlInput = new QLineEdit(this);
    sendButton = new QPushButton("Отправить запрос", this);
    responseLabel = new QLabel("Ответ сервера будет показан здесь", this);

    layout->addWidget(urlLabel);
    layout->addWidget(urlInput);
    layout->addWidget(sendButton);
    layout->addWidget(responseLabel);

    connect(sendButton, &QPushButton::clicked, this, [this]() {
        sendRequest(urlInput->text());
    });

    // Подключение сигналов
    connect(networkManager, &QNetworkAccessManager::finished, this, &Client::onReplyFinished);
}

Client::~Client()
{
}

void Client::sendRequest(const QString &url)
{
    QNetworkRequest request((QUrl(url)));
    networkManager->get(request);
}

void Client::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QString response = reply->readAll();
        qDebug() << "Ответ получен:" << response;
        responseLabel->setText("Ответ: " + response);
    } else {
        QString error = reply->errorString();
        qDebug() << "Ошибка в ответе:" << error;
        responseLabel->setText("Ошибка: " + error);
    }
    reply->deleteLater();
}
