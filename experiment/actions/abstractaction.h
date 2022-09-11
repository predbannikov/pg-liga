#ifndef ABSTRACTACTION_H
#define ABSTRACTACTION_H

#include "global.h"
#include <QDataStream>
#include <QDebug>
#include <QtEndian>
#include <QThread>


enum PROCESS_ACTION {PROCESS_OK, PROCESS_COMPLATE, PROCESS_ERROR};

class AbstractAction
{

public:
    AbstractAction();
    virtual ~AbstractAction();
//    virtual StatusOperation create() = 0;
    virtual PROCESS_ACTION next(StatusOperation &status) = 0;
//    virtual QByteArray getPDU() = 0;
    void readValue(StatusOperation &status, qint64 &value);

    QString statusError = "no error";
//    bool finished = false;
//    QByteArray getPDU();
//    StatusOperation operation;
//    quint8 func_code = 0;
//    quint16 address = 0;
//    quint16 count_reg = 0;
//    quint16 command = 0;
};

#endif // ABSTRACTACTION_H
