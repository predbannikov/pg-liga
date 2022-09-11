#ifndef ACTIONSTEPPERSTOP_H
#define ACTIONSTEPPERSTOP_H

#include "abstractaction.h"

class ActionStepperStop : public AbstractAction
{
    enum STATE {STATE_0x10_WRITE, STATE_0x10_CHECK } state = STATE_0x10_WRITE;
    qint32 data;
    quint16 addressStepper;
    QByteArray req1;
    QByteArray tmp;
public:
    ActionStepperStop(quint8 num_func);
    ~ActionStepperStop();
    PROCESS_ACTION next(StatusOperation &status) override;

};

#endif // ACTIONSTEPPERSTOP_H
