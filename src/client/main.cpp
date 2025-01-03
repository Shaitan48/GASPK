#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Client.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    // Создание объекта клиентского класса
    Client client;

    // Настройка движка QML
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("client", &client);

    engine.load(QUrl(QStringLiteral("MainWindow.qml")));

    return app.exec();
}
