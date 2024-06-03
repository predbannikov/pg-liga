#include "experiment.h"

Experiment::Experiment(quint8 addr) : SequenceExecutor(addr)
{
    modbus = new SerialPort(this);
//    QObject::connect(this, &Interface::sendRequestToModbus, modbus, &SerialPort::parseReqest, Qt::QueuedConnection);
    modbus->process();
}

Experiment::~Experiment()
{
    qDebug() << Q_FUNC_INFO;
}



void Experiment::doWork()
{
    state_machine_experiment();
}

void Experiment::state_machine_experiment()
{
    switch (stat) {
    case Experiment::STATE_IDLE:
        while(execut());
        break;
    case Experiment::STATE_EXPERIMENT:
        break;
    case Experiment::STATE_FINISH:
        break;

    }
}

