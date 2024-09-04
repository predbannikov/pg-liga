#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "operations.h"


class Experiment : public Operations
{
    enum STATUS { STATE_IDLE, STATE_EXPERIMENT, STATE_FINISH } stat = STATE_IDLE;

public:
    Experiment(quint8 addr);
    ~Experiment();
public slots:
    void doWork();
    void experimentParser() override;


private:

    void state_machine_experiment();

};

#endif // EXPERIMENT_H
