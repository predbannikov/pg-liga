#include "interface.h"
#include <QCoreApplication>


Interface::Interface(quint8 addr, QObject *parent) : QObject(parent), address(addr)
{
//    QObject::connect(this, &Interface::sendRequestToModbus, modbus, &SerialPort::parseReqest, Qt::QueuedConnection);

}

Interface::~Interface()
{

}

void Interface::write(QJsonObject &jOperation)
{
    //    jobj["data"] = "01100000000102f100e3c0";
//    QJsonObject jRequest;
    jOperation["action"] = "usually";
//    QString pdu = "0000000102f100";
    jOperation["PDU_request"] = jOperation["request"].toString();
    jOperation["address"] = QString::number(address);
    jOperation["status"] = "wait";
//    emit sendRequestToModbus(jRequest);
    modbus->parseReqest(jOperation);
    qApp->processEvents();
}

bool Interface::read(QJsonObject &jOperation)
{
    do {
        QMutexLocker lock(&mtx);
        while (queueResponse.empty())
            notEmpty.wait(&mtx);


        jobjTaked = QJsonObject(queueResponse.takeFirst());
        execCMD(jobjTaked);
        /*
//        if (jobjTaked["status"] == "" || abort) {
//            return false;
//            // TODO сделать завершение
//        }

        if(jobjTaked["type"].toString() == "modbus") {
            if (jobjTaked.contains("connection")) {
                qDebug() << Q_FUNC_INFO << "modbusserver client " << jobjTaked;
                jobjTaked = QJsonObject();  // не позволяем выйти с цикла
            } else {
                if (jobjTaked["modbus_error"].toString() == "crc_error") {
    //                qDebug() << jResponse["modbus_error"].toString();
                }
                if (jobjTaked["status"].toString() == "complate") {
                    if (statusOperation.request == jobjTaked["PDU_request"].toVariant().toByteArray()) {
                        statusOperation.strError = jobjTaked["modbus_error"].toString();
                        statusOperation.response = jobjTaked["PDU_response"].toVariant().toByteArray();
                    } else {
                        qDebug() << "diff requests" << statusOperation.request
                                 << "\n PDU_request" << jobjTaked["PDU_request"].toVariant().toByteArray()
                                 << "\n PDU_response" << jobjTaked["PDU_response"].toVariant().toByteArray()
                                 << "\n queueResponse.size" << queueResponse.size();
                    }
                }
            }
        } else if (jobjTaked["type"].toString() == "client") {
            // Вся обработка комманд
            execCMD(jobjTaked);

        } else {
            qDebug() << "stop";
        }
        */
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
