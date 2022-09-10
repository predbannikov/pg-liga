/**
  ******************************************************************
  * @file    minibusclient.h
  * @author  ProgressGeo
  * @version V0.1.0.0
  * @date    04.03.2022
  * @brief   Modbus RTU-like serial protocol for Qt
  * @details Содержит класс MinibusClient - обработчик запросов
  *          устройствам, имеющим протокол MiniBus.
  *          Один экземпляр MinibusClient обеспечивает связь со
  *          всеми устройствами, подключенными к одному COM-порту
  ******************************************************************
  */

#ifndef MINIBUSCLIENT_H
#define MINIBUSCLIENT_H

/* ** MINIBUS **
 * A minimal implementation of a Modbus RTU-like serial protocol for Qt (client only)
 * Inspired by Qt's Modbus implementation
 * Not fully Modbus-compliant (and doesn't even try to be)
 * Supports: addressing, reading & writing of 16-bit values, (minimal) error handling.
 */

#include <QQueue>
#include <QObject>
#include <QPointer>
#include <QMutex>
#include <QMap>
#include <QFile>
#include <QThread>
#include <QWaitCondition>
#include <QDebug>
#include <QDateTime>
#include <QDir>


#include "modbusreply.h"

class QTimer;
class QSerialPort;



/** <!-- ---------------------------------------------------- -->
 * \brief The MinibusClient - обработчик запросов
 *          устройствам, имеющим протокол MiniBus
 * \details Один экземпляр MinibusClient обеспечивает связь со
 *          всеми устройствами, подключенными к одному COM-порту.
 *          Опрос Minibus-устройств производится функцией
 *          \ref processQueue, которая периодически вызывается по
 *          таймерам m_requestTimer и m_responseTimer (таймеры
 *          запускаются при создании экземпляра и потом каждый
 *          раз перезапускаются из processQueue.
 */
class ModBusClient : public QObject
{
    Q_OBJECT

    struct QueueElement
    {
        QueueElement () = default;
        QueueElement (ModBusReply *reply, const DataUnit &request, int maxRetryCount):
            reply          (reply),
            requestData    (request.data()),
            retryCountLeft (maxRetryCount)
        {}

        QPointer<ModBusReply> reply;
        QByteArray requestData;
        int retryCountLeft;
        qint64 bytesWritten = 0;
    };

public:
    enum State {
        Idle,
        Schedule,
        Send,
        Receive
    };

    ModBusClient (const QString &portName, bool enableDumping = false, QObject *parent = nullptr);
   ~ModBusClient ();

    /// \brief Посылка запроса на чтение значений группы регистров из Minibus-устройства
    /// \param serverAddress   - адрес устройства (Modbus/Minibus-адрес)
    /// \param startAddress    - адрес регистра, начиная с которого будут считываться значения
    /// \param valueCount      - количество считываемых регистров (размер группы)
    /// \return                - результат исполнения запроса
    /// \details Запрос будет добавлен в очередь запросов
    ModBusReply *sendReadRequest  (quint8 serverAddress, quint16 startAddress, quint16 valueCount);

    /// \brief Посылка запроса на запись значений для группы регистров Minibus-устройства
    /// \param serverAddress    - адрес устройства (Modbus/Minibus-адрес)
    /// \param startAddress     - адрес регистра, начиная с которого будут записываться значения
    /// \param values           - количество регистров (размер группы)
    /// \return                 - результат исполнения запроса
    /// \details Запрос будет добавлен в очередь запросов
    ModBusReply *sendWriteRequest (quint8 serverAddress, quint16 startAddress, const QVector<quint16> &values);

    /// \brief Открытие COM-порта (подключение к порту) если он ещё не открыт
    /// \return - результат открытия
    bool connectDevice    ();
    /// \brief Закрытие COM-порта (отключение от порта) если он открыт
    /// \return - результат закрытия
    void disconnectDevice ();

    QString portName() const;

    int     interFrameDelay () const      { return m_interFrameDelayMs; }
    void setInterFrameDelay (int delayMs) { m_interFrameDelayMs = delayMs; }

    int     requestTimeout () const        { return m_requestTimeoutMs; }
    void setRequestTimeout (int timeoutMs) { m_requestTimeoutMs = timeoutMs; }

    int     responseTimeout () const        { return m_responseTimeoutMs; }
    void setResponseTimeout (int timeoutMs) { m_responseTimeoutMs = timeoutMs; }

    int     userCount () const    { return m_userCount; }
    void setUserCount (int count) { m_userCount = count; }

    int   isClientError () const    { return m_clientError; }
    void setClientError (int error) { m_clientError = error; }

    static QString toString(State state);

private slots:

    /// \brief Циклически вызываемая функция обработки имеющихся запросов
    /// \details Выполняет основную работу класса
    void processQueue ();
    void writeToPort  ();

    void onSerialBytesWritten (qint64 bytes);
    void onSerialReadyRead ();
    void onSerialError ();

signals:

    void connected    ();
    void disconnected ();
    void clientError  (int errorCode);
    void clientReconnected ();

    void queueSizeChanged(int);
    void stateChanged(int);
    void statChanged(QMap<int, int>, int, int);

private:

    ModBusReply *enqueueRequest(const DataUnit &request);
    void scheduleNextRequest();

    void setState(State state);
    void setError(ModBusReply::Error);
    void setSendRetry();
    void setReceiveRetry();

    QQueue<QueueElement> m_queue;
    QueueElement m_current;
    QByteArray m_responseBuffer;

    QSerialPort *m_port;
    QTimer *m_requestTimer;
    QTimer *m_responseTimer;

    State m_state = Idle;

    int m_maxRetryCount = 3;
    int m_interFrameDelayMs = 2;
    int m_requestTimeoutMs = 200;
    int m_responseTimeoutMs = 500;

    int m_userCount = 0;
    int m_clientError = 0;

    QMap<int, int> m_errorCount;
    int m_sendRetryCount = 0;
    int m_receiveRetryCount = 0;
};

#endif // MINIBUSCLIENT_H
