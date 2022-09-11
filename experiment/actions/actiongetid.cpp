#include "actiongetid.h"

ActionGetID::ActionGetID()
{
}

ActionGetID::~ActionGetID()
{

}

PROCESS_ACTION ActionGetID::next(StatusOperation &status)
{
    qint64 value = 0;
    switch (state) {
    case STATE_0x10_WRITE:
        status.request = "100000000102F100";
        state = STATE_0x10_CHECK;
        break;
    case STATE_0x10_CHECK:
        if (status.strError == "no_error")
            state = STATE_0x03_WRITE;
        else
            state = STATE_0x10_CHECK;
        break;
    case STATE_0x03_WRITE:
        status.request = "0300010001";
        state = STATE_0x03_CHECK;
        break;
    case STATE_0x03_CHECK:
        if (status.strError == "no_error") {
            readValue(status, value);
            qDebug() << "address =" << value;
//            status.loadFrame.address = value;
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
