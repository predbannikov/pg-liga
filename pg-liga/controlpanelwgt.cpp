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
    workState = new QState();
    QObject::connect(workState, &QState::entered, this, [this]() {
        ui->btnStartExperiment->setVisible(false);
        ui->btnStopExperiment->setVisible(true);
        ui->btnPauseExperiment->setVisible(true);
        ui->btnContinueExperiment->setVisible(false);
    });

    stopState = new QState();
    QObject::connect(stopState, &QState::entered, this, [this]() {
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
    machine->addState(workState);
    machine->addState(stopState);
    machine->addState(pauseState);

    // Устанавливаем начальное состояние
    machine->setInitialState(workState);

    // Добавляем переходы между состояниями
    workState->addTransition(ui->btnStopExperiment, &QToolButton::clicked, stopState);
    workState->addTransition(ui->btnPauseExperiment, &QToolButton::clicked, pauseState);

    stopState->addTransition(ui->btnStartExperiment, &QToolButton::clicked, workState);

    pauseState->addTransition(ui->btnStopExperiment, &QToolButton::clicked, stopState);
    pauseState->addTransition(ui->btnContinueExperiment, &QToolButton::clicked, workState);

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
