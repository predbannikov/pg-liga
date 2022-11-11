#include "clientexperiment.h"

ClientExperiment::ClientExperiment(QString host_, QObject *parent) : AbstractJSONClient(host_, parent), address(host_.split(':')[1].toUInt()%100)
{

//    connect(socket, &QTcpSocket::readyRead, this, &ClientExperiment::readyRead, Qt::DirectConnection);
//    connect(socket, &QTcpSocket::disconnected, this, &ClientExperiment::disconnected, Qt::DirectConnection);
//    connect(socket, &QTcpSocket::connected, this, &ClientExperiment::connected);
//    connect(socket, &QTcpSocket::bytesWritten, this, &ClientExperiment::bytesWritten);
//    connect(socket, &QTcpSocket::errorOccurred, this, &ClientExperiment::onErrorOccurred);

//    connect(&timerReconnect, &QTimer::timeout, this, &ClientExperiment::connectToHost);
//    timerReconnect.setInterval(700);
//    timerReconnect.start();
}

ClientExperiment::~ClientExperiment()
{
    qDebug() << Q_FUNC_INFO ;
}

qint64 ClientExperiment::timeInterval(const QString& date, const QString& format)
{
    QDateTime sourceDate(QDate(1900, 1, 1), QTime(0, 0, 0));
    QDateTime toDt = QDateTime::fromString(date, format);
    QDateTime interval = QDateTime::fromMSecsSinceEpoch(toDt.toMSecsSinceEpoch() - sourceDate.toMSecsSinceEpoch());
    return interval.toSecsSinceEpoch();
}

void ClientExperiment::readyRead(QJsonObject &jobj)
{
    emit sendReadyResponse(jobj);
}

void ClientExperiment::onSendReadyRequest(QJsonObject jobj)
{
    write(jobj);
}
