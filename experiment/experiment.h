#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "operations.h"

#include "baseaction.h"


class Experiment : public Operations
{
    Q_OBJECT

    enum TRANSITION {TRANSITION_1, TRANSITION_2, TRANSITION_3, TRANSITION_4} transition = TRANSITION_1;
    enum TRANSITION_PAUSE {TRANSITION_PAUSE_1, TRANSITION_PAUSE_2, TRANSITION_PAUSE_3} transition_pause = TRANSITION_PAUSE_1;


public:
    Experiment(quint8 addr);
    ~Experiment();
    void stateSwitch() override;

public slots:
    void doWork();


private:
    bool conveyor();
    bool pause();

    int curAction = 0;
    BaseAction *action = nullptr;

};

#endif // EXPERIMENT_H
