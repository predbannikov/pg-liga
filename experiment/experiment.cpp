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
        qDebug() << "STATE_EXPERIMENT_IDLE";
        break;
    case Experiment::STATE_EXPERIMENT_PROCESS:
        // qDebug() << "STATE_EXPERIMENT_PROCESS";
        if (perform()) {
            stateExperiment = STATE_EXPERIMENT_IDLE;
        }
        break;
    case Experiment::STATE_EXPERIMENT_PAUSE:
        qDebug() << "STATE_EXPERIMENT_PAUSE";
        break;
    }
}

