// main.cpp (Client Entry Point)
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qDebug() << "Client main is running.";
    return app.exec();
}
