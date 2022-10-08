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
    if (statusOperation.request.isEmpty())
        return true;


    write(statusOperation);
//    qDebug() << "write" << statusOperation.request;
    for (int i = 0; i < 3; i++)
        if (read(statusOperation))
            break;
        else if (i == 2)
            return true;
    next();

//    qDebug() << statusOperation.request;
//    qDebug() << statusOperation.response;

    statusOperation.request.clear();
    statusOperation.response.clear();
    statusOperation.strError = "";

    return true;
}

