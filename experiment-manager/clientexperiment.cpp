#include "clientexperiment.h"
#include <QHostAddress>

ClientExperiment::ClientExperiment(quint8 addr, QObject *parent) : QObject(parent), address(addr)
{
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead, this, &ClientExperiment::readyRead, Qt::DirectConnection);
    connect(socket, &QTcpSocket::disconnected, this, &ClientExperiment::disconnected, Qt::DirectConnection);
    connect(socket, &QTcpSocket::connected, this, &ClientExperiment::connected);
    connect(socket, &QTcpSocket::bytesWritten, this, &ClientExperiment::bytesWritten);
    connect(socket, &QTcpSocket::errorOccurred, this, &ClientExperiment::onErrorOccurred);

    connect(&timerReconnect, &QTimer::timeout, this, &ClientExperiment::connectToHost);
    timerReconnect.setInterval(700);
    timerReconnect.start();
}

qint64 ClientExperiment::timeInterval(const QString& date, const QString& format)
{
    QDateTime sourceDate(QDate(1900, 1, 1), QTime(0, 0, 0));
    QDateTime toDt = QDateTime::fromString(date, format);
    QDateTime interval = QDateTime::fromMSecsSinceEpoch(toDt.toMSecsSinceEpoch() - sourceDate.toMSecsSinceEpoch());
    return interval.toSecsSinceEpoch();
}

void ClientExperiment::connected()
{
    timerReconnect.stop();
    qDebug() << Q_FUNC_INFO ;
    emit connectClient();
}

void ClientExperiment::bytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
}

ClientExperiment::~ClientExperiment()
{
    emit deleteClient(address);
    qDebug() << Q_FUNC_INFO ;
}

void ClientExperiment::onErrorOccurred(QAbstractSocket::SocketError error)
{
    qDebug() << Q_FUNC_INFO << "error:" << error;
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

void ClientExperiment::readyRead()
{
    QJsonDocument jdoc = QJsonDocument::fromJson(socket->readAll());
    jresponse = jdoc.object();
    emit sendReadyResponse(jresponse);
}

void ClientExperiment::connectToHost()
{
    if (socket->state() == QAbstractSocket::UnconnectedState) {
        qDebug() << Q_FUNC_INFO << "Connecting,..";
//        socket->connectToHost(QHostAddress::LocalHost, 50000 | address);
        socket->connectToHost("192.168.1.102", 50000 | address);
        if(!socket->waitForConnected(5000))
        {
//            qDebug() << "Error: " << socket->errorString();
        }
    }
//    qDebug() << Q_FUNC_INFO << "connection state" << socket->state();
}

void ClientExperiment::disconnected()
{
//    quint64 isock = reinterpret_cast<quint64>(socket);
    socket->abort();
    qDebug() << Q_FUNC_INFO << "client disconnected";
    timerReconnect.start();
}

void ClientExperiment::onSendReadyRequest(QJsonObject jobj)
{
    QByteArray tmp = QJsonDocument(jobj).toJson();
    if (socket->isValid() && socket->state() == QAbstractSocket::ConnectedState) {
//        qDebug() << socket->state();
        socket->write(tmp);
        if (!socket->waitForBytesWritten(5000)) {
            qDebug() << socket->errorString();
        }
    }
}
