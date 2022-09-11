#include "actionstepperstop.h"

ActionStepperStop::ActionStepperStop(quint8 num_func)
{
    quint16 cmd = 0x2500 | num_func;
    QDataStream str1(&tmp, QIODevice::ReadWrite);
    str1.setByteOrder(QDataStream::BigEndian);
    str1 << static_cast<quint8>(0x10);
    str1 << static_cast<quint16>(0x0);
    str1 << static_cast<quint16>(0x01);
    str1 << static_cast<quint8>(sizeof (cmd));
    str1 << static_cast<quint16>(cmd);
    req1 = tmp.toHex();
}

ActionStepperStop::~ActionStepperStop()
{

}

PROCESS_ACTION ActionStepperStop::next(StatusOperation &status)
{
    switch (state) {
    case STATE_0x10_WRITE:
        status.request = req1;
        state = STATE_0x10_CHECK;
        break;
    case STATE_0x10_CHECK:
        if (status.strError == "no_error") {
            qDebug() << Q_FUNC_INFO << "complate";
            state = STATE_0x10_WRITE;
            return PROCESS_COMPLATE;
        }
        else
            return  PROCESS_ERROR;
        break;
    }
    return PROCESS_OK;
}
