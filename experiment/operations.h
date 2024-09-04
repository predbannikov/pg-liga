#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "global.h"

#include "interface.h"
#include <QJsonObject>
#include <QFile>
#include "operations.h"
#include "global.h"
#include "device.h"


class Plata : public AbstractUnit {

public:
    Plata(quint16 addr_, FunctionCode funCode_) : AbstractUnit(addr_, funCode_){}
    ~Plata(){}

    void setValue(quint16 *data, AbstractUnit::CMD cmd) override {
    Q_UNUSED(data);
    Q_UNUSED(cmd);
    }
};


class Operations : public Interface
{

    enum STATE {STATE_IDLE, STATE_PROCESS} state = STATE_IDLE;


public:
    Operations(quint8 addr);
    ~Operations();

    void resetCommunicationState();
    bool execut();
    virtual void experimentParser() = 0;


    QJsonObject jStatusOperation;
    int counter = 0;
    LoadFrame loadFrame;
    Plata plata;
    RETCODE execCMD(QJsonObject &jobj);


private:


};

#endif // OPERATIONS_H
