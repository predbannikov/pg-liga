#include "operations.h"

#define DEBUG 1

Operations::Operations(quint8 addr) : Interface(addr)
{
}

Operations::~Operations()
{

}

bool Operations::execut()
{
    next();

//    if (DEBUG) {
//        if (sequencer == 0) {
//            sequencer = 1;
//        } else {
//            sequencer = 0;
//            qDebug() << statusOperation.request;
//            qDebug() << statusOperation.response;
//            qDebug() << "";
//        }
//    }

    write(statusOperation);
    if (!read(statusOperation))
        return false;

    next();

//    qDebug() << statusOperation.request;
//    qDebug() << statusOperation.response;

    statusOperation.request.clear();
    statusOperation.response.clear();
    statusOperation.strError = "";

    return true;
}

