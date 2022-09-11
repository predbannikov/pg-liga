#include "abstractunit.h"

RETCODE AbstractUnit::read(StatusOperation &operation, CMD cmd)
{
    // TODO зависнет если постоянно будет ошибка в STATE_0x10_WRITE,
    switch (stateRead) {
    case STATE_0x10_WRITE:
        operation.request = Requests::write(cmd, funCode, address);
        stateRead = STATE_0x10_CHECK;
        break;
    case STATE_0x10_CHECK:
        if (operation.strError == "no_error")
            stateRead = STATE_0x03_WRITE;
        else
            stateRead = STATE_0x10_WRITE;
        break;
    case STATE_0x03_WRITE:
        operation.request = Requests::read(address, DEFAULT_READ_COUNT_REG);
        stateRead = STATE_0x03_CHECK;
        break;
    case STATE_0x03_CHECK:
        if (operation.strError == "no_error") {
            //                qDebug() << "response" << operation.response;
            readValue(operation);
            setValue(values.data(), cmd);
            stateRead = STATE_0x10_WRITE;
            return COMPLATE;
        } else {
            stateRead = STATE_0x03_WRITE;
        }
        break;
    case STATE_COMPLATE:
        qDebug() << "need to next action!!!";
        return ERROR;
        break;
    }
    return NOERROR;
}

RETCODE AbstractUnit::write(StatusOperation &operation, AbstractUnit::CMD cmd)
{
    switch (stateWrite) {
    case AbstractUnit::STATE_0x03_WRITE:
        break;
    case AbstractUnit::STATE_0x03_CHECK:
        break;
    case AbstractUnit::STATE_COMPLATE:
        break;
    case STATE_0x10_WRITE:
        operation.request = Requests::write(cmd, funCode, address);
        stateWrite = STATE_0x10_CHECK;
        break;
    case STATE_0x10_CHECK:
        stateWrite = STATE_0x10_WRITE;
        if (operation.strError == "no_error")
            return COMPLATE;
        else
            return ERROR;
        break;
    }
    return NOERROR;
}

RETCODE AbstractUnit::write(StatusOperation &operation, QVariant data)
{
    switch (stateWrite2) {
    case AbstractUnit::STATE_0x03_WRITE:
        break;
    case AbstractUnit::STATE_0x03_CHECK:
        break;
    case AbstractUnit::STATE_COMPLATE:
        break;
    case STATE_0x10_WRITE:
        operation.request = Requests::write(address, data);
        stateWrite2 = STATE_0x10_CHECK;
        break;
    case STATE_0x10_CHECK:
        stateWrite2 = STATE_0x10_WRITE;
        if (operation.strError == "no_error")
            return COMPLATE;
        else
            return ERROR;
        break;
    }
    return NOERROR;
}





void AbstractUnit::readValue(const StatusOperation &operation)
{
    values.clear();
    QByteArray arr = QByteArray::fromHex(operation.response);
    for(int i = 2; i < arr.size(); i+= sizeof(quint16))
        values.append(qFromBigEndian<quint16>(arr.mid(i, sizeof(quint16))));

}
