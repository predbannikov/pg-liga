#include "uploader.h"
#include "qprocessordetection.h"

Uploader::Uploader(QObject *parent) : QObject(parent)
{
    repos = new ClientRepositarion(parent);
    connect(repos, &ClientRepositarion::connectedClient, this, &Uploader::connectedClientRepositarion);

}

Uploader::~Uploader()
{

}

void Uploader::connectedClientRepositarion()
{
    QDir cur_dir = QDir::current();
    QStringList list = cur_dir.entryList(QDir::Files);

    qDebug() << cur_dir << "\n" << list;

    QFile file_prog;
    file_prog.setFileName("experiment.exe");
    if (!file_prog.open(QIODevice::ReadOnly)) {
        qDebug() << "experiment file not open" << file_prog.fileName();
        return;
    }
    QByteArray prog;
    QDataStream ds(&prog, QIODevice::WriteOnly);
    ds.setVersion(QDataStream::Qt_5_11);
    ds.setByteOrder(QDataStream::BigEndian);
    ds << file_prog.readAll();
    file_prog.close();
    QJsonObject jobj;
    jobj["CMD"] = "upload";
    jobj["VERSION_MAJOR"] = QString(VERSION_MAJOR);
    jobj["VERSION_MINOR"] = QString(VERSION_MINOR);
    jobj["VERSION_BUILD"] = QString(VERSION_BUILD);
    jobj["experiment"] = QString(prog.toBase64());

    repos->onSendReadyRequest(jobj);
}
