#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtGlobal>
#include <QByteArray>
#include <QVariant>
#include <QDataStream>
#include <QtEndian>
#include <QString>
#include <QDebug>

//#include "device.h"

#define DEFAULT_READ_COUNT_REG  0x02


enum RETCODE {NOERROR, COMPLATE, ERROR};

enum PDU_COMMAND {
    PDU_COMMAND_0x03 = 0x03,
    PDU_COMMAND_0x10 = 0x10
};


enum FunctionCode {
    ActBase   = 0,
    ActVol0   = 0,
    ActVol1   = 1,
    ActLoad0  = 2,
    ActLoad1  = 3,
    ActShear0 = 3,

    SensBase  = 4,
    SensPrs0  = 4,
    SensPrs1  = 5,
    SensPrs2  = 6,
    SensLoad0 = 7,
    SensLoad1 = 8,
    SensDef0  = 9,
    SensDef1  = 10,

    CtrlBase   = 11,
    CtrlVol0   = 11,
    CtrlVol1   = 12,
    CtrlLoad0  = 13,
    CtrlLoad1  = 14,
    CtrlShear0 = 14,

    FuncEnd    = 15,
};

enum CommandRegisterAddr : quint16 {
    CmdAddr         = 0,
    IDataAddr       = 1,
    Calib0Addr      = 2,
    Calib1Addr      = 4,

    FuncBaseAddr    = 6,
    ActBaseAddr     = 6,
    ActVol0Addr     = 6,
    ActVol1Addr     = 8,
    ActLoad0Addr    = 10,
    ActLoad1Addr    = 12,
    ActShear0Addr   = 12,

    SensBaseAddr    = 14,
    SensPress0Addr  = 14,
    SensPress1Addr  = 16,
    SensPress2Addr  = 18,
    SensLoad0Addr   = 20,
    SensLoad1Addr   = 22,
    SensDeform0Addr = 24,
    SensDeform1Addr = 26,

    CtrlBaseAddr    = 28,
    CtrlVol0Addr    = 28,
    CtrlVol1Addr    = 30,
    CtrlLoad0Addr   = 32,
    CtrlLoad1Addr   = 34,
    CtrlShear0Addr  = 34,

    RegisterEndAddr = 36,
};


enum OpCode : quint16 {
    OpCodeMask     = 0xff00,
    GetID          = 0xf100,
    GetNumFunc     = 0xf200,
    GetFuncType    = 0xf300,

    SoftReset      = 0xfd00,
    HardReset      = 0xfe00,
    Nop            = 0xff00,

    SensorRead     = 0x1000,
    SensorReadRaw  = 0x1100,
    SensorCalRead  = 0x1200,
    SensorCalStore = 0x1300,
    SensorNull     = 0x1400,
    SensorReset    = 0x1500,

    StepperMove    = 0x2000,
    StepperMoveTo  = 0x2100,
    StepperPos     = 0x2200,
    StepperStatus  = 0x2300,
    StepperSpeed   = 0x2400,
    StepperStop    = 0x2500,
    StepperNull    = 0x2600,

    ControllerSet    = 0x3000,
    ControllerLock   = 0x3100,
    ControllerSetKp  = 0x3200,
    ControllerSetKi  = 0x3300,
    ControllerSetKd  = 0x3400,
    ControllerStatus = 0x3500,
};

struct StatusOperation {
    QByteArray request;
    QByteArray response;
    QString strError;
};


struct Requests {
    static QByteArray write(quint16 cmd, quint8 funcNum, quint16 addr) {
        QByteArray arr;
        QDataStream str(&arr, QIODevice::ReadWrite);
        str.setVersion(QDataStream::Qt_5_11);
        str.setByteOrder(QDataStream::BigEndian);
        str << static_cast<quint8>(PDU_COMMAND_0x10);
        str << static_cast<quint16>(0x00);                  // TODO
        str << static_cast<quint16>(0x01);                  // TODO
        str << static_cast<quint8>(sizeof (cmd));
        str << static_cast<quint16>(cmd | funcNum);
        return arr.toHex();
    }

    static QByteArray read(quint16 addr_, quint16 countReg_) {
        QByteArray arr;
        QDataStream str(&arr, QIODevice::ReadWrite);
        str.setVersion(QDataStream::Qt_5_11);
        str.setByteOrder(QDataStream::BigEndian);
        str << static_cast<quint8>(PDU_COMMAND_0x03);
        str << static_cast<quint16>(addr_);
        str << static_cast<quint16>(countReg_);
        return arr.toHex();
    }

    static QByteArray write(quint16 addr, QVariant data) {
        QByteArray arr;
        QDataStream str(&arr, QIODevice::ReadWrite);
        str.setVersion(QDataStream::Qt_5_11);
        str.setByteOrder(QDataStream::BigEndian);
        str << static_cast<quint8>(PDU_COMMAND_0x10);
        str << static_cast<quint16>(addr);
        QVector<quint16> tmp = modbusData(data);
        int size = tmp.size();
        str << static_cast<quint16>(size);
        str << static_cast<quint8>(size * 2);
        for (auto value: tmp)
            str << static_cast<quint16>(value);
        return arr.toHex();
    }

    static QVector<quint16> modbusData(const QVariant &data)
    {


        QVector<quint16> result;

        const QString dataType = data.typeName();

        /* TODO: Find a better way to convert these values...? */
        if(dataType == "QVector<quint16>") {
            result = data.value<QVector<quint16>>();

        } else if((dataType == "float") || (dataType == "double")) {
            const auto value = data.toFloat();
            auto *ptr = reinterpret_cast<const quint16*>(&value);

            for(unsigned i = 0; i < sizeof(float) / sizeof(quint16); ++i) {
                result.append(ptr[i]);
            }

        } else if((dataType == "int") || (dataType == "unsigned int")) {
            const auto value = data.toInt();
            auto *ptr = reinterpret_cast<const quint16*>(&value);

            for(unsigned i = 0; i < sizeof(int) / sizeof(quint16); ++i) {
                result.append(ptr[i]);
            }

        }  else {
            qDebug() << "undefined type " << Q_FUNC_INFO;
        }



        return result;
    }

};





#endif // GLOBAL_H
