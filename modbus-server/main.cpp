#include <QCoreApplication>
#include <QLockFile>
#include <QDir>

#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QLockFile lockFile(QDir::temp().absoluteFilePath("modbus.lock"));

    /* Пытаемся закрыть Lock File, если попытка безуспешна в течение 100 миллисекунд,
     * значит уже существует Lock File созданный другим процессом.
     * Следовательно, выбрасываем предупреждение и закрываем программу
     * */
    if (!lockFile.tryLock(100))
    {
        qDebug() << "ModbusServer already running, exit program.";
        return 1;
    }

    Server server;
    server.startServer();

    return a.exec();
}
