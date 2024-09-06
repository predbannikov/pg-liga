#include <QJsonObject>
#include "experiment.h"

#include "movebytimeloadframe.h"

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
            transition = TRANSITION_4;
        else {
            curAction = jExperiment["curAction"].toString().toInt();
            transition = TRANSITION_2;
        }
        break;

    case Experiment::TRANSITION_2:
        for (QJsonObject::iterator iter = jExperiment.begin(); iter != jExperiment.end(); ++iter) {
            if (iter.key().toInt() == curAction) {
                qDebug() << "key=" << iter.key() << "    curAction=" << curAction;
                action = new MoveByTimeLoadFrame(this);
                action->initialization(iter.value().toObject(), &loadFrame, &plata);
                transition = TRANSITION_3;
                return false;
            }
        }
        transition = TRANSITION_4;
        break;

    case Experiment::TRANSITION_3:
        if (action->update()) {
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
    switch (transitionToStopDevice) {
    case Experiment::TRANSITION_TO_STOP_DEVICE_1:
    {
        QJsonObject jobj;
        jobj["CMD"] = "unlock_PID";
        put(jobj);
        jobj["CMD"] = "stop_frame";
        put(jobj);
        transitionToStopDevice = TRANSITION_TO_STOP_DEVICE_2;
        loadFramePosition = loadFrame.stepper->position;
    }
        break;
    case Experiment::TRANSITION_TO_STOP_DEVICE_2:
        if (loadFrame.controller->status == 0 && loadFramePosition == loadFrame.stepper->position) {
            transitionToStopDevice = TRANSITION_TO_STOP_DEVICE_1;
            return true;
        }
        loadFramePosition = loadFrame.stepper->position;
        break;
    }
    return false;
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
