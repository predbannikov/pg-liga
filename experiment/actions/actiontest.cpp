#include "actiontest.h"


ActionTest::ActionTest()
{
}

ActionTest::~ActionTest()
{

}

PROCESS_ACTION ActionTest::next(StatusOperation &status)
{
    qint64 value = 0;
    quint16 cmd = 0x1100 | 0x16;
    switch (state) {
    case STATE_0x10_WRITE:
        status.request = "10";
        status.request += "0000";
        status.request += "0001";
        status.request += "02";
//        status.request += "FE00";
        status.request += "1107";
        state = STATE_0x10_CHECK;
        break;
    case STATE_0x10_CHECK:
        if (status.strError == "no_error") {


            state = STATE_0x03_WRITE;
//            qDebug() << status.response;
        }
        else
            state = STATE_0x10_CHECK;
        break;
    case STATE_0x03_WRITE:
        status.request = "0300000002";
        state = STATE_0x03_CHECK;
        break;
    case STATE_0x03_CHECK:
        if (status.strError == "no_error") {
            readValue(status, value);
            qDebug() << "test value =" << value << status.response;
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
