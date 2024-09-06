#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "operations.h"

#include "baseaction.h"


class Experiment : public Operations
{
    Q_OBJECT

    enum TRANSITION {TRANSITION_1, TRANSITION_2, TRANSITION_3, TRANSITION_4} transition = TRANSITION_1;
    enum TRANSITION_TO_PAUSE {TRANSITION_TO_PAUSE_1, TRANSITION_TO_PAUSE_2} transitionToPause = TRANSITION_TO_PAUSE_1;
    enum TRANSITION_TO_STOP {TRANSITION_TO_STOP_1, TRANSITION_TO_STOP_2} transitionToStop = TRANSITION_TO_STOP_1;

    // Перечисление создано для управления устройством, физичиского остановления рамы , возможно стоит перенести реализацию в Operations, а оператионс познее переименовать
    enum TRANSITION_TO_STOP_DEVICE {TRANSITION_TO_STOP_DEVICE_1, TRANSITION_TO_STOP_DEVICE_2} transitionToStopDevice = TRANSITION_TO_STOP_DEVICE_1;

public:
    Experiment(quint8 addr);
    ~Experiment();
    void stateSwitch() override;

public slots:
    void doWork();


private:
    bool conveyor();
    bool pausing();
    bool stopping();
    bool stopDevice();

    int curAction = 0;
    int loadFramePosition = 0;
    BaseAction *action = nullptr;

};

#endif // EXPERIMENT_H
