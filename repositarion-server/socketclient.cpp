#include "socketclient.h"

ClientSocket::ClientSocket(QTcpSocket *sock, QObject *parent) : AbstractJSONClient(sock, parent)
{
}

ClientSocket::~ClientSocket()
{
}

void ClientSocket::readyRead(QJsonObject &jobj)
{
    if (jobj["CMD"].toString() == "check")  {
        qDebug() << "check\n version" << jobj;
    } else if (jobj["CMD"].toString() == "upload"){
        qDebug() << "upload";
        qDebug() << jobj["VERSION_MAJOR"].toString();
        qDebug() << jobj["VERSION_MINOR"].toString();
        qDebug() << jobj["VERSION_BUILD"].toString();
        QByteArray buff = QByteArray::fromBase64(jobj["experiment"].toString().toUtf8());

        QByteArray prog;
        QDataStream ds(&buff, QIODevice::ReadOnly);
        ds >> prog;
        QFile file_prog;
        file_prog.setFileName("experiment.exe");
        if (!file_prog.open(QIODevice::WriteOnly)) {
            qDebug() << "experiment file not open" << file_prog.fileName();
            return;
        }
        file_prog.write(prog);
        file_prog.close();

        QDir cur_dir = QDir::current();
        QStringList list = cur_dir.entryList(QDir::Files);

        qDebug() << "\n" << list;

    }
}

void ClientSocket::sendReadyRequest(QJsonObject &jobj)
{
    write(jobj);
}
