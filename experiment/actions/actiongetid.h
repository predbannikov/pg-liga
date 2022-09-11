#ifndef ACTIONGETID_H
#define ACTIONGETID_H

#include "abstractaction.h"

class ActionGetID : public AbstractAction
{
    enum STATE {STATE_0x10_WRITE, STATE_0x10_CHECK, STATE_0x03_WRITE, STATE_0x03_CHECK, STATE_COMPLATE } state = STATE_0x10_WRITE;
public:
    ActionGetID();
    ~ActionGetID();
    PROCESS_ACTION next(StatusOperation &status) override;
};

#endif // ACTIONGETID_H
