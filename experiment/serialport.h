#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QThread>
#include <QQueue>
#include <QWaitCondition>
#include <QDebug>
#include <QJsonObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QElapsedTimer>
#include <QMutex>

struct CountErrors {
    quint64 NoError = 0;
    quint64 DeviceNotFoundError = 0;
    quint64 PermissionError = 0;
    quint64 OpenError = 0;
    quint64 NotOpenError = 0;
    quint64 ParityError = 0;
    quint64 FramingError = 0;
    quint64 BreakConditionError = 0;
    quint64 WriteError = 0;
    quint64 ReadError = 0;
    quint64 ResourceError = 0;
    quint64 UnsupportedOperationError = 0;
    quint64 TimeoutError = 0;
    quint64 UnknownError = 0;
    quint64 CrcError = 0;
};

enum ResponseSize {
    ExceptionResponseSize = 3,
    ReadResponseSize = 3,
    WriteResponseSize = 6,
    Crc16Size = sizeof(quint16),
};
enum Position {
    ServerAddressPos,
    FunctionCodePos,
    HeaderSize,

    ReadBytesPos = HeaderSize,
    ReadDataPos,

    WriteStartPos = HeaderSize,
    WriteCountPos = WriteStartPos + sizeof(quint16),
};
enum FunctionCodeModeBus : quint8 {
    Read = 0x03,
    Write = 0x10,
    Exception = 0x80
};

struct ModBusReply {
    const int m_maxRequests = 3;
    const int m_timeoutSendRequest = 150;
    int m_currentNumRequest = 0;
};

class SerialPort : public QObject
{
    Q_OBJECT
    QMutex mtx;
    QWaitCondition notEmpty;
//    std::mutex mtx;
//    std::condition_variable notEmpty;

    QQueue<QJsonObject> queue;
    bool abort = false;

    QSerialPort *port;
    CountErrors countErrors;
    QSerialPort::SerialPortError stateErr;

    QElapsedTimer timeElapse;
    QElapsedTimer timeSpentRequest;
    QElapsedTimer timeSpentWait;
    quint64 count_requestes_all = 0;
    quint64 spent_time_to_requests = 0;
    quint64 spent_time_to_wait = 0;
    quint64 spent_time_one_wait = 0;

    bool connectDevice();
    void procError();
    QString writeToPort(QByteArray &req);
    quint16 calculateCRC16(const QByteArray &data);
    void printCountErrors();
    bool checkCRC(const QByteArray &data);
    int expectedResponseLength(const QByteArray &data);
    bool isValidResponse(const QByteArray &data);
public:
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();

    void init();

signals:
    void readyRequest(QJsonObject);

public slots:
    void parseReqest(QJsonObject &jobj);
//    void put(const QJsonObject&);
};

#endif // SERIALPORT_H
