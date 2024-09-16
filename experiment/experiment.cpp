#include <QJsonObject>
#include "experiment.h"

#include "movebytimeloadframe.h"
#include "steppedpressure.h"

Experiment::Experiment(quint8 addr) : Operations(addr)
{
}

Experiment::~Experiment()
{
    qDebug() << Q_FUNC_INFO;
}

void Experiment::doWork()
{
    modbus = new SerialPort();
    modbus->init();
    while(execut());
}

bool Experiment::conveyor()
{
    /** Мы можем читать датчики, но задавать целевые значения и передвижение
            шаговиков, необходимо выполнять через queueRequest (очередь команд)

            jExperiment содержит jActions
            нужно перебрать их все и когда все действия закончатся вернуть true
            что будет означать завершение всех действий

            нужно создать несколько кейсов
                1 кейс получаем номер акшиона
                2 кейс для выборки акшиона
                3 кейс для выполнения акшиона
                4 кейс для завершения эксперимента
        */

    switch (transition) {
    case Experiment::TRANSITION_1:
        if (jExperiment.isEmpty())
            stateExperiment = STATE_EXPERIMENT_IDLE;
        else {
            curAction = jExperiment["curAction"].toString().toInt();
            transition = TRANSITION_2;
        }
        break;

    case Experiment::TRANSITION_2:
        for (QJsonObject::iterator iter = jExperiment.begin(); iter != jExperiment.end(); ++iter) {
            if (iter.key().contains("Operation") && iter.key().split('_')[1].toInt() == curAction) {

                qDebug() << "key=" << iter.key().split('_')[1] << "    curAction=" << curAction;
                if (!createAction(iter.value().toObject()))
                    transition = TRANSITION_4;
                else
                    transition = TRANSITION_3;
                return false;
            }
        }
        transition = TRANSITION_4;
        break;

    case Experiment::TRANSITION_3:
        if (action->updating()) {
            action->finish();
            jExperiment[QString::number(curAction)] = action->jAction;
            curAction++;
            jExperiment["curAction"] = QString::number(curAction);      // Считать как указатель, с которого начнётся выполнение следующей ступени
            delete action;
            action = nullptr;
            transition = TRANSITION_2;
            return false;
        }
        break;

    case Experiment::TRANSITION_4:
        stateExperiment = STATE_EXPERIMENT_TRANSIT_TO_PAUSE;
        transition = TRANSITION_1;
        return false;
        // return true;     // Завершение эксперимента
    }
    return false;
}

bool Experiment::pausing()
{
    /**
     *  Если в данный момент нет созданного акшиона, значит
     *  или эксперимента нет, что значит такого тоже не должно быть
     *  или в данный момент во время эксперимента идёт переключение
     *  в конвейере, а во время переключения все рамы должны быть остановлены
     */


    switch (transitionToPause) {
    case Experiment::TRANSITION_TO_PAUSE_1:
        if (action != nullptr) {
            if (action->pause())
                transitionToPause = TRANSITION_TO_PAUSE_2;
        } else {
            transitionToPause = TRANSITION_TO_PAUSE_2;
        }
        break;
    case Experiment::TRANSITION_TO_PAUSE_2:
        if (stopDevice()) {
            transitionToPause  = TRANSITION_TO_PAUSE_1;
            return true;
        }
        break;
    }
    return false;
}

bool Experiment::stopping()
{
    switch (transitionToStop) {
    case Experiment::TRANSITION_TO_STOP_1:
        curAction = 0;
        jExperiment = QJsonObject();
        transitionToStop = TRANSITION_TO_STOP_2;
        transition = TRANSITION_1;
        transitionToPause = TRANSITION_TO_PAUSE_1;
        if (action != nullptr)
            delete action;
        break;
    case Experiment::TRANSITION_TO_STOP_2:
        if (stopDevice()) {
            transitionToStop = TRANSITION_TO_STOP_1;
            return true;
        }
        break;

    }
    return false;
}

bool Experiment::stopDevice()
{
    QJsonObject jobj;

    switch (transitionToStopDevice) {

    case Experiment::TRANSITION_TO_STOP_LOADFRAME_1:
         jobj["CMD"] = "load_frame_unlock_PID";
        put(jobj);
        jobj["CMD"] = "load_frame_stop";
        put(jobj);
        transitionToStopDevice = TRANSITION_TO_STOP_LOADFRAME_2;
        loadFramePosition = loadFrame->stepper->position;
        break;
    case Experiment::TRANSITION_TO_STOP_LOADFRAME_2:
        if (loadFrame->controller->status == 0 && loadFramePosition == loadFrame->stepper->position) {
            transitionToStopDevice = TRANSITION_TO_STOP_VOLUMETER1_1;
        }
        loadFramePosition = loadFrame->stepper->position;
        break;
    case Experiment::TRANSITION_TO_STOP_VOLUMETER1_1:
        jobj["CMD"] = "volumetr1_unlock_PID";
        put(jobj);
        jobj["CMD"] = "volumetr1_stop";
        put(jobj);
        transitionToStopDevice = TRANSITION_TO_STOP_VOLUMETER1_2;
        volumeter1Position = volumetr1->stepper->position;
        break;
    case Experiment::TRANSITION_TO_STOP_VOLUMETER1_2:
        if (volumetr1->controller->status == 0 && volumeter1Position == volumetr1->stepper->position) {
            transitionToStopDevice = TRANSITION_TO_STOP_VOLUMETER2_1;
        }
        volumeter1Position = volumetr1->stepper->position;
        break;
    case Experiment::TRANSITION_TO_STOP_VOLUMETER2_1:
        jobj["CMD"] = "volumetr2_unlock_PID";
        put(jobj);
        jobj["CMD"] = "volumetr2_stop";
        put(jobj);
        transitionToStopDevice = TRANSITION_TO_STOP_VOLUMETER2_2;
        volumeter2Position = volumetr2->stepper->position;
        break;
    case Experiment::TRANSITION_TO_STOP_VOLUMETER2_2:
        if (volumetr2->controller->status == 0 && volumeter2Position == volumetr2->stepper->position) {
            transitionToStopDevice = TRANSITION_TO_STOP_LOADFRAME_1;
            return true;
        }
        volumeter2Position = volumetr2->stepper->position;
        break;
    }
    return false;
}

bool Experiment::createAction(QJsonObject jAction)
{
    if (jAction["name"].toString() == "move_of_time") {
        action = new MoveByTimeLoadFrame(this);
    } else if (jAction["name"].toString() == "steppedPressure") {
        action = new SteppedPressure(this);
    } else {
        return false;
    }
    connect(action, &BaseAction::error, this, &Interface::sendRequestToClient);
    action->initialization(jAction, loadFrame, volumetr1, volumetr2, &plata);
    return true;
}

void Experiment::deleteAction()
{
    disconnect(action, &BaseAction::error, this, &Interface::sendRequestToClient);
    delete action;
    action = nullptr;
}

void Experiment::stateSwitch()
{
    switch (stateExperiment) {
    case Experiment::STATE_EXPERIMENT_IDLE:
        // qDebug() << "STATE_EXPERIMENT_IDLE";
        break;
    case Experiment::STATE_EXPERIMENT_PROCESS:
        // qDebug() << "STATE_EXPERIMENT_PROCESS";
        if (conveyor()) {
            stateExperiment = STATE_EXPERIMENT_IDLE;
        }
        break;
    case Experiment::STATE_EXPERIMENT_PAUSE:

        // qDebug() << "STATE_EXPERIMENT_PAUSE";
        break;
    case Operations::STATE_EXPERIMENT_TRANSIT_TO_PAUSE:
        qDebug() << "STATE_EXPERIMENT_TRANSIT_TO_PAUSE";
        if (pausing())
            stateExperiment = STATE_EXPERIMENT_PAUSE;
        break;
    case Operations::STATE_EXPERIMENT_TRANSIT_TO_STOP:
        if (stopping())
            stateExperiment = STATE_EXPERIMENT_IDLE;
        break;
    }
}
