#ifndef ACTIONSTEPPERSPEED_H
#define ACTIONSTEPPERSPEED_H

#include "abstractaction.h"

class ActionStepperSpeed : public AbstractAction
{
    enum STATE {STATE_0x10_WRITE, STATE_0x10_CHECK, STATE_0x03_WRITE, STATE_0x03_CHECK, STATE_COMPLATE } state = STATE_0x10_WRITE;
    qint32 data;
    quint16 addressStepper;
    QByteArray req1;
    QByteArray req2;
    QByteArray tmp;

public:
    ActionStepperSpeed(qint32 data_, quint16 addr_param, quint16 num_func);
    ~ActionStepperSpeed();
    PROCESS_ACTION next(StatusOperation &status) override;

};

#endif // ACTIONSTEPPERSPEED_H
