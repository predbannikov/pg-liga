#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "global.h"

#include "interface.h"
#include <QJsonObject>
#include <QFile>
#include "operations.h"
#include "global.h"
#include "device.h"

class Operations : public Interface
{

    enum STATE {STATE_IDLE, STATE_PROCESS} state = STATE_IDLE;
    int sequencer = 0;
    LoadFrame loadFrame;

public:
    Operations(quint8 addr);
    ~Operations();
//    virtual RETCODE next() = 0;
//    virtual void resetCommunicationState() = 0;
    RETCODE next();
    void resetCommunicationState();

    bool execut();


    QJsonObject jStatusOperation;
    int counter = 0;

    RETCODE execCMD(QJsonObject &jobj);


private:


private:

};

#endif // OPERATIONS_H
