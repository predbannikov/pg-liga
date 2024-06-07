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
    RETCODE ret = next();
    if (ret != NOERROR) {
        resetCommunicationState();
//        qDebug() << "Читаем следующую команду из списка если она есть, меняем состояние на выполнение" << retcodeToStr(ret);
    } else {
        if (jStatusOperation["request"].toString().isEmpty())
            return true;
        write(jStatusOperation);    // send to modbas
    }


//    write(jStatusOperation);    // send to modbas

//    qDebug() << retcodeToStr(next());

//    jStatusOperation = QJsonObject();
    return true;
}

