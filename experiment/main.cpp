#include <QCoreApplication>
#include <QThread>
#include <QLockFile>
#include <memory>
//#include "stdlib.h"

#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::unique_ptr<QLockFile>lockFile;
    try {
        if (argc == 2) {
            int addr = atoi(argv[1]);
            if (addr > 0 && addr < 32) {
                QString lockFilePath = QDir::temp().absoluteFilePath(QString("experiment%1.lock").arg(addr));
                std::unique_ptr<QLockFile>lf = std::unique_ptr<QLockFile>(new QLockFile(QDir::temp().absoluteFilePath(lockFilePath)));
                lockFile = std::move(lf);
                /* Пытаемся закрыть Lock File, если попытка безуспешна в течение 100 миллисекунд,
                 * значит уже существует Lock File созданный другим процессом.
                 * Следовательно, выбрасываем предупреждение и закрываем программу
                 * */
                if (!lockFile->tryLock(1000))
                {
                    qDebug() << lockFilePath << "already running, exit program.";
                    return 0x01;
                }
                Server *server = new Server(addr, &a);
                server->startServer();
            } else {
                qDebug() << "support address from range [1..31]";
            }
        } else {
            qDebug() << "required set one parameter numb address";
            return 0;
        }

    }  catch (...) {
        qDebug() << "ERROR: app throw ";
        return 0x10;
    }


    return a.exec();
}
