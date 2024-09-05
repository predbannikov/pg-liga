#include <QDebug>
#include <QMovie>
#include <QBuffer>
#include <QPixmap>

#include "controlpanelwgt.h"
#include "ui_controlpanelwgt.h"

ControlPanelWgt::ControlPanelWgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanelWgt)
{
    ui->setupUi(this);


    machine = new QStateMachine(this);

    // Создаем состояния
    experimentState = new QState();
    QObject::connect(experimentState, &QState::entered, this, [this]() {
        ui->btnStartExperiment->setVisible(false);
        ui->btnStopExperiment->setVisible(true);
        ui->btnPauseExperiment->setVisible(true);
        ui->btnContinueExperiment->setVisible(false);
    });

    idleState = new QState();
    QObject::connect(idleState, &QState::entered, this, [this]() {
        ui->btnStartExperiment->setVisible(true);
        ui->btnStopExperiment->setVisible(false);
        ui->btnPauseExperiment->setVisible(false);
        ui->btnContinueExperiment->setVisible(false);
    });

    pauseState = new QState();
    QObject::connect(pauseState, &QState::entered, this, [this]() {
        ui->btnStartExperiment->setVisible(false);
        ui->btnStopExperiment->setVisible(true);
        ui->btnPauseExperiment->setVisible(false);
        ui->btnContinueExperiment->setVisible(true);
    });


    // Добавляем состояния в машину
    machine->addState(experimentState);
    machine->addState(idleState);
    machine->addState(pauseState);

    // Устанавливаем начальное состояние
    machine->setInitialState(idleState);

    // Добавляем переходы между состояниями
    experimentState->addTransition(this, &ControlPanelWgt::transitToIdle, idleState);
    experimentState->addTransition(this, &ControlPanelWgt::transitToPause, pauseState);

    idleState->addTransition(this, &ControlPanelWgt::transitToProcess, experimentState);

    pauseState->addTransition(this, &ControlPanelWgt::transitToIdle, idleState);
    pauseState->addTransition(this, &ControlPanelWgt::transitToProcess, experimentState);

    connect(ui->btnStartExperiment, &QToolButton::clicked, this, &ControlPanelWgt::btnStart);
    connect(ui->btnStopExperiment, &QToolButton::clicked, this, &ControlPanelWgt::btnStop);
    connect(ui->btnPauseExperiment, &QToolButton::clicked, this, &ControlPanelWgt::btnPause);
    connect(ui->btnContinueExperiment, &QToolButton::clicked, this, &ControlPanelWgt::btnContinue);


    // Запускаем машину
    machine->start();
}

ControlPanelWgt::~ControlPanelWgt()
{
    delete ui;
}

void ControlPanelWgt::changeButtons(QString newState)
{
    if (newState == "STATE_EXPERIMENT_IDLE") {
        emit transitToIdle();
    } else if (newState == "STATE_EXPERIMENT_PROCESS") {
        emit transitToProcess();
    } else if (newState == "STATE_EXPERIMENT_PAUSE") {
        emit transitToPause();
    }
}
