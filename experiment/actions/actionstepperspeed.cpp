#include "actionstepperspeed.h"

ActionStepperSpeed::ActionStepperSpeed(qint32 data_, quint16 addr_param, quint16 num_func) : data(data_), addressStepper(addr_param)
{
    quint16 cmd = 0x2400 | num_func;
    QDataStream str(&tmp, QIODevice::ReadWrite);
    str.setByteOrder(QDataStream::BigEndian);
    str << static_cast<quint8>(0x10);
    str << static_cast<quint16>(addressStepper);
    str << static_cast<quint16>(num_func);
    str << static_cast<quint8>(sizeof (data));
    str << static_cast<quint32>(data);
    req1 = tmp.toHex();

    tmp.clear();
    QDataStream str2(&tmp, QIODevice::ReadWrite);
    str2.setByteOrder(QDataStream::BigEndian);
    str2 << static_cast<quint8>(0x10);
    str2 << static_cast<quint16>(0x0);
    str2 << static_cast<quint16>(0x01);
    str2 << static_cast<quint8>(sizeof (cmd));
    str2 << static_cast<quint16>(cmd);
    req2 = tmp.toHex();


}

ActionStepperSpeed::~ActionStepperSpeed()
{

}

PROCESS_ACTION ActionStepperSpeed::next(StatusOperation &status)
{
    switch (state) {
    case STATE_0x10_WRITE:
        status.request = req1;
        state = STATE_0x10_CHECK;
        break;
    case STATE_0x10_CHECK:
        if (status.strError == "no_error")
            state = STATE_0x03_WRITE;
        else
            state = STATE_0x10_CHECK;
        break;
    case STATE_0x03_WRITE:
        status.request = req2;
        state = STATE_0x03_CHECK;
        break;
    case STATE_0x03_CHECK:
        if (status.strError == "no_error") {
            qDebug() << Q_FUNC_INFO << "complate";
            state = STATE_COMPLATE;
        } else {
            state = STATE_0x10_WRITE;
        }
        return PROCESS_COMPLATE;
    case STATE_COMPLATE:
        qDebug() << "need to next action!!!";
        return PROCESS_ERROR;
    }
    return PROCESS_OK;
}
