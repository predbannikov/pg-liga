#include "clientrepositarion.h"
#include <QHostAddress>

ClientRepositarion::ClientRepositarion(QObject *parent) : AbstractJSONClient("176.119.147.253:51234", parent)
{

}

ClientRepositarion::~ClientRepositarion()
{
    qDebug() << Q_FUNC_INFO ;
}

void ClientRepositarion::readyRead(QJsonObject &jobj)
{
    emit sendReadyResponse(jobj);
}

void ClientRepositarion::onSendReadyRequest(QJsonObject jobj)
{
    write(jobj);
}
