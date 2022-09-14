#include <QCoreApplication>
#include <QThread>

#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server(1, &a);
    server.startServer();

    return a.exec();
}
