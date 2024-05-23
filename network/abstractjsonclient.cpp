#include "abstractjsonclient.h"

AbstractJSONClient::AbstractJSONClient(QString endPoint, QObject *parent) : QObject(parent), host(endPoint.split(':')[0]), port(endPoint.split(':')[1].toInt())
{
    QHostInfo hostInfo = QHostInfo::fromName(endPoint.split(':')[0]);

    if (hostInfo.error() != QHostInfo::NoError) {
        qDebug() << Q_FUNC_INFO << hostInfo.errorString();
        return;
    }
    host = hostInfo.addresses().first();
    type = CLIENT;
    socket = new QTcpSocket(this);

    if (socket == nullptr) {
        qDebug() << typeToStr(type) << "socket not created";
    } else {
        QMetaObject::invokeMethod(this, &AbstractJSONClient::init, Qt::QueuedConnection);
    }

}

AbstractJSONClient::AbstractJSONClient(QTcpSocket *socket_, QObject *parent) : QObject(parent)
{
    type = SERVER;
    socket = socket_;

    if (socket == nullptr) {
        qDebug() << typeToStr(type) << "socket not created";
    } else {
        QMetaObject::invokeMethod(this, &AbstractJSONClient::init, Qt::QueuedConnection);
    }
}

AbstractJSONClient::~AbstractJSONClient()
{
    qDebug() << Q_FUNC_INFO;
}

void AbstractJSONClient::connectToHost()
{
    if (socket->state() == QAbstractSocket::UnconnectedState) {
        qDebug() << Q_FUNC_INFO << "Connecting,.." << QString("%1:%2").arg(host.toString()).arg(port);
        socket->connectToHost(host, port);
//        if(!socket->waitForConnected(timeout))
//        {
//            qDebug() << "Error: " << socket->errorString();
//        }
    }
}

bool AbstractJSONClient::write(QJsonObject &jobj)
{
    QByteArray req = QJsonDocument(jobj).toJson();
    QByteArray buff;
    QDataStream ds(&buff, QIODevice::ReadWrite);
    ds.setVersion(QDataStream::Qt_5_11);
    ds.setByteOrder(QDataStream::BigEndian);
    ds << req;
    if (socket->isValid() && socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(buff);
        return true;
    }
    return false;
}

QString AbstractJSONClient::typeToStr(TYPE tp)
{
    if (tp == CLIENT)
        return "CLIENT";
    else if (tp == SERVER)
        return "SERVER";
    return "TYPE NOT DEFINED";
}

void AbstractJSONClient::onReadyRead()
{
    QDataStream dss(socket);
    QByteArray arr;
    dss.setVersion(QDataStream::Qt_5_11);
    dss.setByteOrder(QDataStream::BigEndian);
    do {
        dss.startTransaction();
        dss >> arr;
        transact = dss.commitTransaction();
        if (transact) {
            QJsonObject jobj = QJsonDocument::fromJson(arr).object();
            switch (type) {
            case SERVER:
                if (jobj.contains("ping")) {
                    jobj["pong"];
                    write(jobj);
                } else {
                    readyRead(jobj);
                }
                break;
            case CLIENT:
                clientLastError = "no_error";
                statusConnection = true;
                timerPing.stop();
                timerPing.start();
                if (!jobj.contains("pong"))
                    readyRead(jobj);
                break;
            }
        }
    } while(transact);
}

void AbstractJSONClient::setTimeoutConnection(int ms)
{
    timeout = ms;
}

void AbstractJSONClient::setTimerReconnect(int ms)
{
    if (timerReconnect.isActive()) {
        timerReconnect.stop();
        timerReconnect.setInterval(ms);
        timerReconnect.start();
    } else {
        timerReconnect.setInterval(ms);
    }
}

void AbstractJSONClient::setTimerPing(int ms)
{
    if (timerPing.isActive()) {
        timerPing.stop();
        timerPing.setInterval(ms);
        timerPing.start();
    } else {
        timerPing.setInterval(ms);
    }
}

qint64 AbstractJSONClient::socketID()
{
    return reinterpret_cast<qint64>(socket);
}

void AbstractJSONClient::init()
{
    if (socket == nullptr) {
        qDebug() << Q_FUNC_INFO << "socket is nullptr";
        return;
    }
    connect(socket, &QTcpSocket::readyRead, this, &AbstractJSONClient::onReadyRead, Qt::DirectConnection);
    connect(socket, &QTcpSocket::disconnected, this, &AbstractJSONClient::disconnected, Qt::DirectConnection);
    connect(socket, &QTcpSocket::connected, this, &AbstractJSONClient::connected, Qt::DirectConnection);
    connect(socket, &QTcpSocket::bytesWritten, this, &AbstractJSONClient::bytesWritten, Qt::DirectConnection);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &AbstractJSONClient::onErrorOccurred, Qt::DirectConnection);

    if (type == CLIENT) {
        connect(&timerReconnect, &QTimer::timeout, this, &AbstractJSONClient::connectToHost, Qt::DirectConnection);
        connect(&timerPing, &QTimer::timeout, this, &AbstractJSONClient::ping, Qt::DirectConnection);
        setTimerReconnect();
        setTimerPing();
        connectToHost();
        timerReconnect.start();
    }
}

void AbstractJSONClient::onErrorOccurred(QAbstractSocket::SocketError error)
{
//    qDebug() << Q_FUNC_INFO << "error:" << error;
    clientLastError = socket->errorString();
    switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
        socket->close();
        break;
    case QAbstractSocket::RemoteHostClosedError:
        socket->close();
        break;
    case QAbstractSocket::HostNotFoundError:
        socket->close();
        break;
    case QAbstractSocket::SocketAccessError:
        socket->close();
        break;
    case QAbstractSocket::SocketResourceError:
        socket->close();
        break;
    case QAbstractSocket::SocketTimeoutError:
//        socket->abort();
        // TODO handled this case
        break;
    case QAbstractSocket::DatagramTooLargeError:
        socket->close();
        break;
    case QAbstractSocket::NetworkError:
        socket->close();
        break;
    case QAbstractSocket::AddressInUseError:
        socket->close();
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        socket->close();
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        socket->close();
        break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        socket->close();
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        socket->close();
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        socket->close();
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        socket->close();
        break;
    case QAbstractSocket::ProxyConnectionClosedError:
        socket->close();
        break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        socket->close();
        break;
    case QAbstractSocket::ProxyNotFoundError:
        socket->close();
        break;
    case QAbstractSocket::ProxyProtocolError:
        socket->close();
        break;
    case QAbstractSocket::OperationError:
        socket->close();
        break;
    case QAbstractSocket::SslInternalError:
        socket->close();
        break;
    case QAbstractSocket::SslInvalidUserDataError:
        socket->close();
        break;
    case QAbstractSocket::TemporaryError:
        socket->close();
        break;
    case QAbstractSocket::UnknownSocketError:
        socket->close();
        break;
    }
}

void AbstractJSONClient::disconnected()
{
    qDebug() << Q_FUNC_INFO << "client disconnected";
    statusConnection = false;

    switch (type) {
    case SERVER:
        emit finished(reinterpret_cast<qint64>(socket));
        break;
    case CLIENT:
        emit disconnectClient(reinterpret_cast<qint64>(socket));
        socket->abort();
        timerPing.stop();
        timerReconnect.start();
        break;
    }
}

void AbstractJSONClient::connected()
{
    qDebug() << Q_FUNC_INFO ;
    statusConnection = true;
    timerReconnect.stop();
    timerPing.start();
    emit connectedClient();
}

void AbstractJSONClient::bytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
}

void AbstractJSONClient::ping()
{
    if (!statusConnection) {
        clientLastError = "Server did not respond to ping";
        socket->close();
    } else {
        statusConnection = false;
        QJsonObject jobj;
        jobj["ping"];
        write(jobj);
    }
}
