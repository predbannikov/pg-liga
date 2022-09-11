#ifndef ABSTRACTUNIT_H
#define ABSTRACTUNIT_H

#include "global.h"
#include "measurements.h"



class AbstractUnit
{
    enum STATE {STATE_0x10_WRITE, STATE_0x10_CHECK, STATE_0x03_WRITE, STATE_0x03_CHECK, STATE_COMPLATE };
    quint16 address = 0x0;
    QVector<quint16> values;


    void readValue(const StatusOperation &operation);

protected:
    enum CMD : quint16 {
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
    RETCODE ret = ERROR;
    STATE stateRead = STATE_0x10_WRITE;
    STATE stateWrite = STATE_0x10_WRITE;
    STATE stateWrite2 = STATE_0x10_WRITE;

public:
    AbstractUnit(quint16 addr_, FunctionCode funCode_) :address(addr_), funCode(funCode_) {};
    ~AbstractUnit() {}


    RETCODE read(StatusOperation &operation, CMD cmd);

    RETCODE write(StatusOperation &operation, CMD cmd);

    RETCODE write(StatusOperation &operation, QVariant data);

    virtual void setValue(quint16 *data, CMD cmd) = 0;

    FunctionCode funCode = ActBase;
};

#endif // ABSTRACTUNIT_H
