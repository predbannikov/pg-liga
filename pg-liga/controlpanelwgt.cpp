#include <QDebug>

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
    experimentState->addTransition(ui->btnStopExperiment, &QToolButton::clicked, idleState);
    experimentState->addTransition(ui->btnPauseExperiment, &QToolButton::clicked, pauseState);

    idleState->addTransition(ui->btnStartExperiment, &QToolButton::clicked, experimentState);

    pauseState->addTransition(ui->btnStopExperiment, &QToolButton::clicked, idleState);
    pauseState->addTransition(ui->btnContinueExperiment, &QToolButton::clicked, experimentState);

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
