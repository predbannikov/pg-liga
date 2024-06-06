#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "sequenceexecutor.h"


class Experiment : public SequenceExecutor
{
    enum STATUS { STATE_IDLE, STATE_EXPERIMENT, STATE_FINISH } stat = STATE_IDLE;

public:
    Experiment(quint8 addr);
    ~Experiment();
public slots:
    void doWork();


private:

    void state_machine_experiment();

};

#endif // EXPERIMENT_H
