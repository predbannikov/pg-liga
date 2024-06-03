#include "modbusclient.h"

#include <QDebug>

#include <QTimer>
#include <QDateTime>
#include <QSerialPort>
#include <QDateTime>

//#include "modbusreply.h"
#include <QHash>

// ----------------------------------------------------------
ModBusClient::ModBusClient(const QString &portName, bool enableDumping, QObject *parent):
    QObject(parent),
    m_port(new QSerialPort(portName, this)),
    m_requestTimer(new QTimer(this)),
    m_responseTimer(new QTimer(this))
{
    /* Hardcoded for now */
    /// \warning  !!!ATT!!! Настройки COM-порта жёстко заданы здесь
    m_port->setFlowControl(QSerialPort::NoFlowControl);
    m_port->setBaudRate   (QSerialPort::Baud115200   );
    m_port->setParity     (QSerialPort::EvenParity   );
    m_port->setStopBits   (QSerialPort::OneStop      );
    m_port->setDataBits   (QSerialPort::Data8        );

    m_requestTimer ->setSingleShot (true);
    m_responseTimer->setSingleShot (true);

//    m_requestTimer->setTimerType(Qt::PreciseTimer);
//    m_responseTimer->setTimerType(Qt::PreciseTimer);

    connect (m_port, &QSerialPort::readyRead    , this, &ModBusClient::onSerialReadyRead   );
    connect (m_port, &QSerialPort::bytesWritten , this, &ModBusClient::onSerialBytesWritten);
    connect (m_port, &QSerialPort::errorOccurred, this, &ModBusClient::onSerialError       );

    connect (m_requestTimer , &QTimer::timeout, this, &ModBusClient::processQueue);
    connect (m_responseTimer, &QTimer::timeout, this, &ModBusClient::processQueue);



}

// ----------------------------------------------------------
ModBusClient::~ModBusClient()
{
    // TODO:
    // Do some cleanup in case of deletion while there are items in queue
    disconnectDevice();
    qDebug() << Q_FUNC_INFO << m_port->portName();
}

// ----------------------------------------------------------
ModBusReply *ModBusClient::sendReadRequest(quint8 serverAddress, quint16 startAddress, quint16 valueCount)
{
    return enqueueRequest(DataUnit(serverAddress, startAddress, valueCount));
}

// ----------------------------------------------------------
ModBusReply *ModBusClient::sendWriteRequest(quint8 serverAddress, quint16 startAddress, const QVector<quint16> &values)
{
    return enqueueRequest(DataUnit(serverAddress, startAddress, values));
}

// ----------------------------------------------------------
bool ModBusClient::connectDevice ()
{
    if(!(m_port->isOpen())) {
        if(m_port->open(QIODevice::ReadWrite)) {
            m_port->setBreakEnabled(false);
            emit connected();
            qDebug() << Q_FUNC_INFO << m_port->portName() << "OPEN";
            return true;
        }
    }
    qDebug() << Q_FUNC_INFO << m_port->portName() << "already OPEN";
    return false;
}

// ----------------------------------------------------------
void ModBusClient::disconnectDevice()
{
    if(m_port->isOpen()) {
        emit disconnected();
        m_port->close();
        QThread::msleep(1);
        qDebug() << Q_FUNC_INFO << m_port->portName() << "disconnected and CLOSE";
    } else {
        qDebug() << Q_FUNC_INFO << m_port->portName() << "already CLOSED";
    }
}

// ----------------------------------------------------------
QString ModBusClient::portName() const
{
    return m_port->portName();
}

QString ModBusClient::toString(State state)
{
    static QHash<State,QString> values = {
        {Idle, "Idle"},
        {Schedule, "Schedule"},
        {Send, "Send"},
        {Receive, "Receive"},
    };
    return values.value(state);
}

// ----------------------------------------------------------
// Циклически вызываемая функция обработки имеющихся запросов
// Выполняет основную работу класса
// ----------------------------------------------------------
void ModBusClient::processQueue ()
{
    switch(m_state) {
    case Schedule:
        if(!m_queue.isEmpty()) {
            m_current = m_queue.dequeue();
            emit queueSizeChanged(m_queue.size());

            if(m_current.reply) {
                QTimer::singleShot(m_interFrameDelayMs, Qt::PreciseTimer, this, &ModBusClient::writeToPort);
            } else {
                QTimer::singleShot(0, this, &ModBusClient::processQueue);
            }

        } else {
            setState(Idle);
        }

        break;

    case Send:
        if(m_current.reply) {
            if(m_current.retryCountLeft > 0) {
                QTimer::singleShot(m_interFrameDelayMs, Qt::PreciseTimer, this, &ModBusClient::writeToPort);
                setSendRetry();
            } else {
                if(m_current.bytesWritten == 0) {
                    m_current.reply->setError(ModBusReply::PortError);
                    setError(ModBusReply::PortError);
                } else {
                    m_current.reply->setError(ModBusReply::RequestTimeoutError);
                    setError(ModBusReply::RequestTimeoutError);
                }

                m_current.reply->setFinished(true);
                scheduleNextRequest();
            }

        } else {
            scheduleNextRequest();
        }

        break;

    case Receive:
        if(m_current.reply) {
            if(m_current.retryCountLeft > 0) {
                QTimer::singleShot(0, this, &ModBusClient::writeToPort);
                setReceiveRetry();
            } else {
                m_current.reply->setError(ModBusReply::ResponseTimeoutError);
                m_current.reply->setFinished(true);
                setError(ModBusReply::ResponseTimeoutError);
                scheduleNextRequest();
            }

        } else {
            scheduleNextRequest();
        }

        break;

    case Idle:
        break;
    }
}

// ----------------------------------------------------------
void ModBusClient::writeToPort()
{
    setState(Send);
    m_responseBuffer.clear();
    m_port->clear(QSerialPort::AllDirections);

    m_current.bytesWritten = 0;
    m_current.retryCountLeft--;

    m_port->write(m_current.requestData);
    m_port->flush();


    m_requestTimer->start(m_requestTimeoutMs);
}

// ----------------------------------------------------------
void ModBusClient::onSerialBytesWritten(qint64 bytes)
{
    m_current.bytesWritten += bytes;

    if(m_current.bytesWritten == m_current.requestData.size()) {
        m_requestTimer->stop();
        setState(Receive);
        m_responseTimer->start(m_responseTimeoutMs);
    }
}

// ----------------------------------------------------------
void ModBusClient::onSerialReadyRead()
{
    if(m_state != Receive) {
        m_port->clear(QSerialPort::AllDirections);
        setError(ModBusReply::UnexpectedReadyRead);
        if (m_current.reply) {
            m_current.reply->setFinished(true);
        }
        return;
    }

    QByteArray part_response = m_port->readAll();
    m_responseBuffer += part_response;

    if (!DataUnit::isValidResponse(m_responseBuffer)) {
        if (m_current.reply) {
            m_current.reply->setError(ModBusReply::InvalidResponse);
            m_current.reply->setFinished(true);
        }
        setError(ModBusReply::InvalidResponse);
        m_port->clear(QSerialPort::AllDirections);
        m_responseTimer->stop();
        scheduleNextRequest();
    }

    if(m_responseBuffer.size() < DataUnit::expectedResponseLength(m_responseBuffer))
        return;

    /* At this point the received data frame is complete*/

    if(DataUnit::checkCRC(m_responseBuffer)) {
        if (m_current.reply) {
            m_current.reply->setResult(DataUnit(m_responseBuffer));
            setError(ModBusReply::NoError);
        } else {
            setError(ModBusReply::ReplyIsNull);
        }
    } else {
        if (m_current.reply) {
        m_current.reply->setError(ModBusReply::CrcError);
        setError(ModBusReply::CrcError);
        } else {
            setError(ModBusReply::ReplyIsNull);
        }
    }

    if (m_current.reply) {
        m_current.reply->setFinished(true);
    } else {
        setError(ModBusReply::ReplyIsNull);
    }

    m_responseTimer->stop();
    scheduleNextRequest();
}

// ----------------------------------------------------------
void ModBusClient::onSerialError ()
{
    emit clientError (m_port->error());
    qCritical() << "[MinibusClient] Error:" << m_port->portName() << m_port->error() << m_port->errorString();
    setClientError (m_port->error());
}

// ----------------------------------------------------------
ModBusReply *ModBusClient::enqueueRequest(const DataUnit &request)
{
    auto *reply = new ModBusReply;
    m_queue.enqueue(QueueElement(reply, request, m_maxRetryCount));
    emit queueSizeChanged(m_queue.size());

    if(m_state == Idle) {
        scheduleNextRequest();
    }

    return reply;
}

// ----------------------------------------------------------
void ModBusClient::scheduleNextRequest()
{
    setState(Schedule);
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
    QMetaObject::invokeMethod(this, "processQueue", Qt::QueuedConnection);
#else
    QMetaObject::invokeMethod(this, &ModBusClient::processQueue, Qt::QueuedConnection);
#endif
}

void ModBusClient::setState(State state)
{
    m_state = state;
    emit stateChanged(m_state);
}

void ModBusClient::setError(ModBusReply::Error error)
{
    if (!m_errorCount.contains(error)) {
        m_errorCount[error] = 0;
    }
    m_errorCount[error] += 1;
    emit statChanged(m_errorCount, m_sendRetryCount, m_receiveRetryCount);
}

void ModBusClient::setSendRetry()
{
    m_sendRetryCount += 1;
    emit statChanged(m_errorCount, m_sendRetryCount, m_receiveRetryCount);
}

void ModBusClient::setReceiveRetry()
{
    m_receiveRetryCount += 1;
    emit statChanged(m_errorCount, m_sendRetryCount, m_receiveRetryCount);
}
// ----------------------------------------------------------
