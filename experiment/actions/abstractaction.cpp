#include "abstractaction.h"
#include <QtEndian>

AbstractAction::AbstractAction()
{

}

AbstractAction::~AbstractAction()
{

}

void AbstractAction::readValue(StatusOperation &status, qint64 &value)
{
    QByteArray arr = QByteArray::fromHex(status.response);
    quint8 size = arr[1];
    if (size == arr.size() - 2) {
        switch (size) {
        case 2:
            value = qFromBigEndian<quint16>(arr.data() + 2);
            break;
        case 4:
            value = qFromBigEndian<quint32>(arr.data() + 2);
            break;
        }
    }

}

//QByteArray AbstractAction::getPDU() {
//    QByteArray pdu;
////    QDataStream str(&pdu, QIODevice::WriteOnly);
////    str << operation.func_code;
////    str << operation.address;
////    str << operation.count_reg;
////    str << static_cast<quint8>(sizeof (operation.count_reg));
////    str << operation.command;
//    return pdu;
//}
