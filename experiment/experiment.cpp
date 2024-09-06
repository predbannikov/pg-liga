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
            delete action;
            transition = TRANSITION_2;
            return false;
        }
        break;

    case Experiment::TRANSITION_4:
        transition = TRANSITION_1;
        return true;
    }
    return false;
}

bool Experiment::pause()
{
    switch (transition_pause) {
    case Experiment::TRANSITION_PAUSE_1:
        break;
    case Experiment::TRANSITION_PAUSE_2:
        break;
    case Experiment::TRANSITION_PAUSE_3:
        break;

    }
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
        // if (pause()) {

        // }
        // qDebug() << "STATE_EXPERIMENT_PAUSE";
        break;
    }
}
