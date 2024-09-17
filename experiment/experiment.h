#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "operations.h"

#include "baseaction.h"

/**
 * @brief The Experiment class      После старта состояния процесса, перебирается
 *
 */


class Experiment : public Operations
{
    Q_OBJECT

    enum TRANSITION {TRANSITION_1, TRANSITION_2, TRANSITION_3, TRANSITION_4} transition = TRANSITION_1;
    enum TRANSITION_TO_PAUSE {TRANSITION_TO_PAUSE_1, TRANSITION_TO_PAUSE_2} transitionToPause = TRANSITION_TO_PAUSE_1;
    enum TRANSITION_TO_STOP {TRANSITION_TO_STOP_1, TRANSITION_TO_STOP_2} transitionToStop = TRANSITION_TO_STOP_1;

    // Перечисление создано для управления устройством, физичиского остановления рамы , возможно стоит перенести реализацию в Operations, а оператионс познее переименовать
    enum TRANSITION_TO_STOP_DEVICE {
        TRANSITION_TO_STOP_LOADFRAME_1, TRANSITION_TO_STOP_LOADFRAME_2,
        TRANSITION_TO_STOP_VOLUMETER1_1, TRANSITION_TO_STOP_VOLUMETER1_2,
        TRANSITION_TO_STOP_VOLUMETER2_1, TRANSITION_TO_STOP_VOLUMETER2_2,
    } transitionToStopDevice = TRANSITION_TO_STOP_LOADFRAME_1;

public:
    Experiment(QString serial_port_name, quint8 addr);
    ~Experiment();
    void stateSwitch() override;

public slots:
    void doWork();


private:
    bool conveyor();
    bool pausing();
    bool stopping();
    bool stopDevice();
    bool createAction(QJsonObject jAction);
    void deleteAction();
    void updateJExperiment();

    int curAction = 0;
    int loadFramePosition = 0;
    int volumeter1Position = 0;
    int volumeter2Position = 0;
    BaseAction *action = nullptr;
    QString portName;
};

#endif // EXPERIMENT_H
