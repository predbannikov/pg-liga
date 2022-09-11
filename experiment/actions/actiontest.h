#ifndef ACTIONTEST_H
#define ACTIONTEST_H

#include "abstractaction.h"

class ActionTest : public AbstractAction
{
    enum STATE {STATE_0x10_WRITE, STATE_0x10_CHECK, STATE_0x03_WRITE, STATE_0x03_CHECK, STATE_COMPLATE } state = STATE_0x10_WRITE;
public:
    ActionTest();
    ~ActionTest();
    PROCESS_ACTION next(StatusOperation &status) override;

};

#endif // ACTIONTEST_H
