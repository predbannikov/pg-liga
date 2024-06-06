#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "global.h"

#include "interface.h"

class Operations : public Interface
{

    int sequencer = 0;
public:
    Operations(quint8 addr);
    ~Operations();
    virtual void next() = 0;

    bool execut();


    QJsonObject jStatusOperation;

private:

};

#endif // OPERATIONS_H
