#include <QCoreApplication>
#include <QThread>

#include "stdlib.h"

#include "server.h"

Server *server;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    try {
        if (argc == 2) {
            int addr = atoi(argv[1]);
            if (addr > 0 && addr < 32) {
                server = new Server(addr, &a);
                server->startServer();
            } else {
                qDebug() << "support address from range [1..31]";
            }
        } else {
            qDebug() << "required set one parameter numb address";
        }

    }  catch (...) {
        qDebug() << "ERROR: app throw ";
    }


    return a.exec();
}
