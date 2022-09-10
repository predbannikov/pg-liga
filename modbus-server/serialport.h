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
    quint64 NoError;
    quint64 DeviceNotFoundError;
    quint64 PermissionError;
    quint64 OpenError;
    quint64 NotOpenError;
    quint64 ParityError;
    quint64 FramingError;
    quint64 BreakConditionError;
    quint64 WriteError;
    quint64 ReadError;
    quint64 ResourceError;
    quint64 UnsupportedOperationError;
    quint64 TimeoutError;
    quint64 UnknownError;
    quint64 CrcError;
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
enum FunctionCode : quint8 {
    Read = 0x03,
    Write = 0x10,
    Exception = 0x80
};

struct ModBusReply {
    const int m_maxRequests = 3;
    const int m_timeoutSendRequest = 50;
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
    void parseReqest(QJsonObject &jobj);
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

    void process();

signals:
    void readyRequest(QJsonObject);

public slots:
    void put(QJsonObject&);
};

#endif // SERIALPORT_H
