#include "interface.h"
#include <QCoreApplication>


Interface::Interface(quint8 addr, QObject *parent) : QObject(parent), address(addr)
{

}

Interface::~Interface()
{

}

void Interface::write(StatusOperation &statusOperation)
{
    //    jobj["data"] = "01100000000102f100e3c0";
    QJsonObject jRequest;
    jRequest["action"] = "usually";
//    QString pdu = "0000000102f100";
    jRequest["PDU_request"] = QString(statusOperation.request);
    jRequest["address"] = QString::number(address);
    jRequest["status"] = "wait";
    emit sendRequestToModbus(jRequest);
    qApp->processEvents();
}

bool Interface::read(StatusOperation &statusOperation)
{
    do {
        QMutexLocker lock(&mtx);
        while (queueResponse.empty())
            notEmpty.wait(&mtx);


        jobjTaked = QJsonObject(queueResponse.takeFirst());

        if (jobjTaked["status"] == "" || abort) {
            return false;
            // TODO сделать завершение
        }

        if(jobjTaked["type"].toString() == "modbus") {
            if (jobjTaked.contains("connection")) {
                qDebug() << Q_FUNC_INFO << "modbusserver client " << jobjTaked;
                jobjTaked = QJsonObject();  // не позволяем выйти с цикла
            } else {
                if (jobjTaked["modbus_error"].toString() == "crc_error") {
    //                qDebug() << jResponse["modbus_error"].toString();
                }
                if (jobjTaked["status"].toString() == "complate") {
                    statusOperation.strError = jobjTaked["modbus_error"].toString();
                    statusOperation.response = jobjTaked["PDU_response"].toVariant().toByteArray();
                }
            }
        } else if (jobjTaked["type"].toString() == "client") {
            // Вся обработка комманд
            execCMD(jobjTaked);

        } else {
            qDebug() << "stop";
        }
    } while(jobjTaked["type"].toString() != "modbus");

    return true;
}

void Interface::put(QJsonObject &jobj)
{
    QMutexLocker lock(&mtx);
    queueResponse.push_back(std::move(jobj));
    notEmpty.notify_one();
}

void Interface::stop()
{
    abort = true;
}
