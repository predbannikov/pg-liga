#include "socketclient.h"

ClientSocket::ClientSocket(QTcpSocket *sock, SerialPort *serial_port, QObject *parent) : AbstractJSONClient(sock, parent)
{
    serialport = serial_port;
//    connect(socket, &QTcpSocket::readyRead, this, &ClientSocket::readyRead, Qt::DirectConnection);
//    connect(socket, &QTcpSocket::disconnected, this, &ClientSocket::disconnected, Qt::DirectConnection);
}

ClientSocket::~ClientSocket()
{
//    qDebug() << Q_FUNC_INFO << "descriptor" ;
}

void ClientSocket::readyRead(QJsonObject &jobj)
{
    jobj["id_experiment"] = QString::number(socketID());
    serialport->put(jobj);
}

//void ClientSocket::disconnected()
//{
//    quint64 isock = reinterpret_cast<quint64>(socket);
//    socket->deleteLater();
//    socket = nullptr;
//    qDebug() << "client disconnected";
//    emit disconnectClient(isock);
//}

void ClientSocket::sendReadyRequest(QJsonObject &jobj)
{
    jobj["type"] = "modbus";
    write(jobj);
}
