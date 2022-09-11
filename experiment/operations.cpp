#include "operations.h"
#include <QDataStream>

#define DEBUG 0

Operations::Operations(quint8 addr) : Interface(addr)
{
}

Operations::~Operations()
{

}

bool Operations::execut()
{
    next();

    if (DEBUG) {
        if (sequencer == 0) {
            sequencer = 1;
        } else {
            sequencer = 0;
            qDebug() << statusOperation.request;
            qDebug() << statusOperation.response;
            qDebug() << "";
        }
    }

    write(statusOperation);
    if (!read(statusOperation))
        return false;

//    next();

    return true;
}

