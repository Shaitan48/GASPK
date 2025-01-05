#include <QApplication>
#include "Client.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Client client;

    return app.exec();
}
