#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <QState>
#include <QStateMachine>
#include <QFinalState>

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

    QStateMachine *machine = nullptr;

    QState *experimentState = nullptr;
    QState *pauseState = nullptr;
    QState *idleState = nullptr;

};

#endif // EXPERIMENT_H
