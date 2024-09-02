#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "global.h"

#include "interface.h"
#include <QJsonObject>
#include <QFile>
#include "operations.h"
#include "global.h"
#include "device.h"


class Plata {

public:
    enum STATE {STATE_0x10_WRITE, STATE_0x10_CHECK, STATE_0x03_WRITE, STATE_0x03_CHECK, STATE_COMPLATE };
    quint16 address = 0x0;


    RETCODE ret = ERROR;
    STATE stateWrite = STATE_0x10_WRITE;
    FunctionCode funCode = ActBase;


    RETCODE write(QJsonObject &jOperation, OpCode cmd)
    {
        switch (stateWrite) {
        case STATE_0x03_WRITE:
            break;
        case STATE_0x03_CHECK:
            break;
        case STATE_COMPLATE:
            break;
        case STATE_0x10_WRITE:
            jOperation["PDU_request"] = QString(Requests::write(cmd, funCode, address));
            stateWrite = STATE_0x10_CHECK;
            break;
        case STATE_0x10_CHECK:
            stateWrite = STATE_0x10_WRITE;
            if (jOperation["modbus_error"].toString() == "no_error")
                return COMPLATE;
            else
                return ERROR;
            break;
        }
        return NOERROR;
    }
};


class Operations : public Interface
{

    enum STATE {STATE_IDLE, STATE_PROCESS} state = STATE_IDLE;
    int sequencer = 0;
    LoadFrame loadFrame;
    Plata plata;

public:
    Operations(quint8 addr);
    ~Operations();
//    virtual RETCODE next() = 0;
//    virtual void resetCommunicationState() = 0;
    RETCODE next();
    void resetCommunicationState();

    bool execut();


    QJsonObject jStatusOperation;
    int counter = 0;

    RETCODE execCMD(QJsonObject &jobj);


private:


private:

};

#endif // OPERATIONS_H
