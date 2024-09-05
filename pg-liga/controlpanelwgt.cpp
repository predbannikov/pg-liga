#include <QDebug>
#include <QMovie>
#include <QBuffer>
#include <QPixmap>

#include "controlpanelwgt.h"
#include "ui_controlpanelwgt.h"

#include "waitingspinnerwidget.h"

ControlPanelWgt::ControlPanelWgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanelWgt)
{
    ui->setupUi(this);

//    WaitingSpinnerWidget* spinner = new WaitingSpinnerWidget(this);

     ui->widget->setRoundness(70.0);
     ui->widget->setMinimumTrailOpacity(15.0);
     ui->widget->setTrailFadePercentage(70.0);
     ui->widget->setNumberOfLines(10);
     ui->widget->setLineLength(5);
     ui->widget->setLineWidth(3);
     ui->widget->setInnerRadius(6);
     ui->widget->setRevolutionsPerSecond(1);
     ui->widget->setColor(QColor(81, 4, 71));
     ui->widget->start(); // gets the show on the road!

    machine = new QStateMachine(this);

    // Создаем состояния
    startState = new QState();
    QObject::connect(startState, &QState::entered, this, [this]() {
        qDebug() << "startState";
        ui->btnStartExperiment->setVisible(false);
        ui->btnStopExperiment->setVisible(false);
        ui->btnPauseExperiment->setVisible(false);
        ui->btnContinueExperiment->setVisible(false);
    });

    disctonnectState = new QState();
    QObject::connect(disctonnectState, &QState::entered, this, [this]() {
        qDebug() << "disctonnectState";
        setEnabled(false);
    });
    connectionState = new QState();
    QObject::connect(connectionState, &QState::entered, this, [this]() {
        qDebug() << "connectionState";
        setEnabled(true);
    });

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
    machine->addState(startState);
    machine->addState(connectionState);
    machine->addState(disctonnectState);
    machine->addState(experimentState);
    machine->addState(idleState);
    machine->addState(pauseState);

    // Устанавливаем начальное состояние
    machine->setInitialState(startState);

    // Добавляем переходы между состояниями


    startState->addTransition(this, &ControlPanelWgt::transitToIdle, idleState);
    startState->addTransition(this, &ControlPanelWgt::transitToPause, pauseState);
    startState->addTransition(this, &ControlPanelWgt::transitToProcess, experimentState);
    startState->addTransition(this, &ControlPanelWgt::transitToDisconnect, disctonnectState);
    startState->addTransition(this, &ControlPanelWgt::transitToConnect, connectionState);

    connectionState->addTransition(this, &ControlPanelWgt::transitToIdle, idleState);
    connectionState->addTransition(this, &ControlPanelWgt::transitToPause, pauseState);
    connectionState->addTransition(this, &ControlPanelWgt::transitToProcess, experimentState);
    connectionState->addTransition(this, &ControlPanelWgt::transitToDisconnect, disctonnectState);

    disctonnectState->addTransition(this, &ControlPanelWgt::transitToConnect, connectionState);

    experimentState->addTransition(this, &ControlPanelWgt::transitToIdle, idleState);
    experimentState->addTransition(this, &ControlPanelWgt::transitToPause, pauseState);
    experimentState->addTransition(this, &ControlPanelWgt::transitToDisconnect, disctonnectState);

    idleState->addTransition(this, &ControlPanelWgt::transitToProcess, experimentState);
    idleState->addTransition(this, &ControlPanelWgt::transitToDisconnect, disctonnectState);

    pauseState->addTransition(this, &ControlPanelWgt::transitToIdle, idleState);
    pauseState->addTransition(this, &ControlPanelWgt::transitToProcess, experimentState);
    pauseState->addTransition(this, &ControlPanelWgt::transitToDisconnect, disctonnectState);

    connect(ui->btnStartExperiment, &QToolButton::clicked, [=](){
        emit btnStart();
        ui->widget->start();
    });
    connect(ui->btnStopExperiment, &QToolButton::clicked, [=](){
        emit btnStop();
        ui->widget->start();
    });
    connect(ui->btnPauseExperiment, &QToolButton::clicked, [=](){
        emit btnPause();
        ui->widget->start();
    });
    connect(ui->btnContinueExperiment, &QToolButton::clicked, [=](){
        emit btnContinue();
        ui->widget->start();
    });


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
    ui->widget->stop();
}

void ControlPanelWgt::setConnectionState(bool state)
{
    if (state) {
        emit transitToConnect();
    } else {
        ui->widget->start();
        emit transitToDisconnect();
        qDebug() << "emit transitToDisconnect();";
    }
}
