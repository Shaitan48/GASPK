#include <QApplication>
#include "Client.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Client client;
    client.setWindowTitle("Клиент GASPK");
    client.resize(400, 300);
    client.show();

    return app.exec();
}
