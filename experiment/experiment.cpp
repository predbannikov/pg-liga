#include <QJsonObject>
#include "experiment.h"

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



void Experiment::parser()
{
    switch (stateExperiment) {
    case Experiment::STATE_EXPERIMENT_IDLE:
        // qDebug() << "STATE_EXPERIMENT_IDLE";
        break;
    case Experiment::STATE_EXPERIMENT_PROCESS:
        // qDebug() << "STATE_EXPERIMENT_PROCESS";
        if (perform()) {
            stateExperiment = STATE_EXPERIMENT_IDLE;
        }
        break;
    case Experiment::STATE_EXPERIMENT_PAUSE:
        // qDebug() << "STATE_EXPERIMENT_PAUSE";
        break;
    }
}


bool ActionMoveOfTime::update() {
    switch(trans) {
    case ActionMoveOfTime::TRANS_1: {
        QJsonObject jobj;
        jobj["CMD"] = "move_frame";
        jobj["speed"] = QString::number(speed);
        putQueue(jobj);
        elapseTime.start();
        trans = TRANS_2;
        break;
    }
    case ActionMoveOfTime::TRANS_2: {
        if (!elapseTime.isActive()) {
            QJsonObject jobj;
            jobj["CMD"] = "stop_frame";
            putQueue(jobj);
            return true;
        }
        break;
    }
    }
    return false;
}

void ActionMoveOfTime::finishing() {
}

void Action::initialization(QJsonObject jAct, LoadFrame *lf, Plata *plt) {
    loadFrame = lf;
    plata = plt;
    jAction = jAct;
    jAction["status"] = "process";
    init();
}
