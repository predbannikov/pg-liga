#include "experiment.h"

Experiment::Experiment(quint8 addr) : Operations(addr)
{
    machine = new QStateMachine(this);

    experimentState = new QState();
    QObject::connect(experimentState, &QState::entered, this, [this]() {

    });

    idleState = new QState();
    QObject::connect(idleState, &QState::entered, this, [this]() {

    });

    pauseState = new QState();
    QObject::connect(pauseState, &QState::entered, this, [this]() {

    });

    machine->addState(experimentState);
    machine->addState(idleState);
    machine->addState(pauseState);

    machine->setInitialState(idleState);

    machine->start();
}

Experiment::~Experiment()
{
    qDebug() << Q_FUNC_INFO;
}

void Experiment::doWork()
{
    modbus = new SerialPort();
    modbus->init();

    state_machine_experiment();
}

void Experiment::experimentParser()
{


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

