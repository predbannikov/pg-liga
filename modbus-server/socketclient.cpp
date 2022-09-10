#include "socketclient.h"

ClientSocket::ClientSocket(QTcpSocket *sock, SerialPort *serial_port, QObject *parent) : QThread(parent)
{
    socket = sock;
    serialport = serial_port;
    connect(socket, &QTcpSocket::readyRead, this, &ClientSocket::readyRead, Qt::DirectConnection);
    connect(socket, &QTcpSocket::disconnected, this, &ClientSocket::disconnected, Qt::DirectConnection);
}

ClientSocket::~ClientSocket()
{
//    qDebug() << Q_FUNC_INFO << "descriptor" ;
}

void ClientSocket::readyRead()
{
    QJsonDocument jdoc = QJsonDocument::fromJson(socket->readAll());
    jrequest = jdoc.object();
//    if (jrequest["action"] == "usually") {

//    } else if (jrequest["action"] == "service") {

//    }

    jrequest["id_experiment"] = QString::number(reinterpret_cast<quint64>(socket));
    serialport->put(jrequest);
}

void ClientSocket::disconnected()
{
    quint64 isock = reinterpret_cast<quint64>(socket);
    socket->deleteLater();
    socket = nullptr;
    qDebug() << "client disconnected";
    emit disconnectClient(isock);
}

void ClientSocket::sendReadyRequest(QJsonObject jobj)
{
    jobj["type"] = "modbus";
    QByteArray tmp = QJsonDocument(jobj).toJson();
    if (socket != nullptr && socket->isOpen())
        socket->write(tmp);
}
