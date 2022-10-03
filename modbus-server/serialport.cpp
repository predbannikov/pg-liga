#include <QCoreApplication>
#include <QDataStream>
#include <QtEndian>

#include "serialport.h"

SerialPort::SerialPort(QObject *parent) : QObject(parent)
{
    timeElapse.start();
    timeSpentRequest.start();
    timeSpentWait.start();
}

bool SerialPort::connectDevice ()
{
    if(!(port->isOpen())) {
        if(port->open(QIODevice::ReadWrite)) {
            port->setBreakEnabled(false);
            qDebug() << Q_FUNC_INFO << port->portName() << "OPEN";
            return true;
        }
    }
    qDebug() << Q_FUNC_INFO << port->portName() << port->errorString();
    return false;
}

void SerialPort::parseReqest(QJsonObject &jobj)
{
    if (jobj["action"].toString() == "usually") {
        QByteArray pdu = QByteArray::fromHex(jobj["PDU_request"].toString().toUtf8());
        quint8 address = jobj["address"].toString().toInt();
        QByteArray modbuspack;
        QDataStream str(&modbuspack, QIODevice::WriteOnly);
        str.setVersion(QDataStream::Qt_5_11);
        str << address;
        str.writeRawData(pdu.data(), pdu.size());
        str.setByteOrder(QDataStream::LittleEndian);
        str << calculateCRC16(modbuspack);
        jobj["port_error"] = writeToPort(modbuspack);           // WRITE
        jobj["status"] = "complate";
        if (jobj["port_error"].toString() == "no_error") {
            if (!modbuspack.isEmpty()) {
                if (checkCRC(modbuspack)) {
                    modbuspack.remove(modbuspack.size() - 2, modbuspack.size());
                    modbuspack.remove(0, 1);
                    jobj["modbus_error"] = "no_error";
                } else {
                    jobj["modbus_error"] = "crc_error";
                    countErrors.CrcError++;
                }
            }
        } else {
            jobj["modbus_error"] = jobj["port_error"].toString();
        }

        jobj["PDU_response"] = QString(modbuspack.toHex());

    } else if(jobj["action"].toString() == "service") {
        QJsonObject jservice = jobj["object"].toObject();
        if (jservice["action"].toString() == "scan") {

        } else if (jservice["action"].toString() == "smth") {

        }
    }
}

void SerialPort::procError()
{
    switch (stateErr) {
    case QSerialPort::NoError:
        countErrors.NoError++;
        break;
    case QSerialPort::DeviceNotFoundError:
        countErrors.DeviceNotFoundError++;
        break;
    case QSerialPort::PermissionError:
        countErrors.PermissionError++;
        break;
    case QSerialPort::OpenError:
        countErrors.OpenError++;
        break;
    case QSerialPort::ParityError:
        countErrors.ParityError++;
        break;
    case QSerialPort::FramingError:
        countErrors.FramingError++;
        break;
    case QSerialPort::BreakConditionError:
        countErrors.BreakConditionError++;
        break;
    case QSerialPort::WriteError:
        countErrors.WriteError++;
        break;
    case QSerialPort::ReadError:
        countErrors.ReadError++;
        break;
    case QSerialPort::ResourceError:
        countErrors.ResourceError++;
        break;
    case QSerialPort::UnsupportedOperationError:
        countErrors.UnsupportedOperationError++;
        break;
    case QSerialPort::UnknownError:
        countErrors.UnknownError++;
        break;
    case QSerialPort::TimeoutError:
        countErrors.TimeoutError++;
        break;
    case QSerialPort::NotOpenError:
        countErrors.NotOpenError++;
        break;
    }

    if (port->error() != QSerialPort::NoError)
    {
        port->clearError();
    }
}

QString SerialPort::writeToPort(QByteArray &req)
{
    QString portError;
    ModBusReply reply;
    while (reply.m_currentNumRequest < reply.m_maxRequests) {
        port->clear(QSerialPort::AllDirections);
        timeSpentRequest.restart();
        port->write(req);
        if (!port->waitForReadyRead(reply.m_timeoutSendRequest)) {        // таймоут или ошибка
            reply.m_currentNumRequest++;
            countErrors.TimeoutError++;
            portError = port->errorString();
            port->clearError();
        } else {
            if (port->error() == QSerialPort::NoError)
                spent_time_to_requests += timeSpentRequest.elapsed();
            portError = "no_error";
            req = port->readAll();
            break;
        }
        req.clear();
    }
    stateErr = port->error();
    return portError;    // TODO переделать на возвращение ошибок только от протокола Modbus ADU (CRC, нет ответа, ...)
}

int SerialPort::expectedResponseLength(const QByteArray &data)
{
    if(data.size() < HeaderSize) {
        return std::numeric_limits<int>::max();
    }

    const auto functionCode = data.at(FunctionCodePos);

    if(functionCode & Exception) {
        return ExceptionResponseSize + Crc16Size;
    } else if((functionCode == Read) && (data.size() >= ReadResponseSize)) {
        return ReadResponseSize + data.at(ReadBytesPos) + Crc16Size;
    } else if(functionCode == Write) {
        return WriteResponseSize + Crc16Size;
    } else {}

    return std::numeric_limits<int>::max(); // Return a huge number to simplify the comparison
}

bool SerialPort::isValidResponse(const QByteArray &data) {
    if(data.size() < HeaderSize) {
        return true;
    }
    const auto functionCode = data.at(FunctionCodePos);
    if(functionCode & Exception || (functionCode == Read) || (functionCode == Write)) {
        return true;
    }
    return false;
}

void SerialPort::process()
{
    port = new QSerialPort(this);
    port->setPortName("ttyUSB1");
    port->setFlowControl(QSerialPort::NoFlowControl);
    port->setBaudRate   (QSerialPort::Baud115200);
    port->setParity     (QSerialPort::EvenParity);
    port->setStopBits   (QSerialPort::OneStop);
    port->setDataBits   (QSerialPort::Data8);
    connectDevice();
    while(!abort) {
        QMutexLocker lock(&mtx);
//        std::unique_lock<std::mutex> lock(mtx);
        while (queue.empty()) {
            notEmpty.wait(&mtx);
        }

        QJsonObject jobj(queue.takeFirst());
        if (jobj["status"] == "") {
            abort = true;
            continue;
        }

        spent_time_one_wait = timeSpentWait.elapsed();
        spent_time_to_wait += timeSpentWait.elapsed();
        parseReqest(jobj);

        timeSpentWait.restart();

        if (timeElapse.elapsed() > 1000) {
            printCountErrors();
            timeElapse.restart();
        }

        procError();

        emit readyRequest(jobj);

        count_requestes_all++;

        qApp->processEvents();
    }
}

void SerialPort::put(QJsonObject &jobj)
{
    QMutexLocker lock(&mtx);
    queue.push_back(std::move(jobj));
    notEmpty.notify_one();
}


SerialPort::~SerialPort()
{
    qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
    port->close();
}

quint16 SerialPort::calculateCRC16 (const QByteArray &data)
{
    quint16 crc = 0xFFFF;
    for (int pos = 0; pos < data.length(); pos++)
    {
        quint8 byte = data[pos];
        crc ^= (quint16) byte;               // XOR byte into least sig. byte of crc

        for (int i = 8; i != 0; i--)         // Loop over each bit
        {
            if ((crc & 0x0001) != 0)         // If the LSB is set
            {
                crc >>= 1;                    // Shift right and XOR 0xA001
                crc ^= 0xA001;
            }
            else                              // Else LSB is not set
                crc >>= 1;                    // Just shift right
        }
    }
    return crc;
}

bool SerialPort::checkCRC(const QByteArray &data)
{
    const auto received = qFromLittleEndian<quint16>(data.right(sizeof(quint16)));
    const auto calculated = calculateCRC16(data.left(data.size() - sizeof(quint16)));
    return received == calculated;
}

void SerialPort::printCountErrors()
{
    qDebug() << countErrors.NoError << "NoError";
    qDebug() << countErrors.DeviceNotFoundError << "DeviceNotFoundError";
    qDebug() << countErrors.PermissionError << "PermissionError";
    qDebug() << countErrors.OpenError << "OpenError";
    qDebug() << countErrors.NotOpenError << "NotOpenError";
    qDebug() << countErrors.ParityError << "ParityError";
    qDebug() << countErrors.FramingError << "FramingError";
    qDebug() << countErrors.BreakConditionError << "BreakConditionError";
    qDebug() << countErrors.WriteError << "WriteError";
    qDebug() << countErrors.ReadError << "ReadError";
    qDebug() << countErrors.ResourceError << "ResourceError";
    qDebug() << countErrors.UnsupportedOperationError << "UnsupportedOperationError";
    qDebug() << countErrors.TimeoutError << "TimeoutError";
    qDebug() << countErrors.UnknownError << "UnknownError\n---modbus-error---";
    qDebug() << countErrors.CrcError << "CrcError";
    if (countErrors.NoError != 0)
        qDebug() << spent_time_to_requests / countErrors.NoError << "avr_spent_time";
    if (count_requestes_all != 0)
        qDebug() << spent_time_to_wait / count_requestes_all << "avr_spent_to_wait";
    qDebug() << spent_time_one_wait << "spent_time_one_wait";
    qDebug() << count_requestes_all << "count_requestes_all\n";
}
