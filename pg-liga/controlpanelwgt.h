#ifndef CONTROLPANELWGT_H
#define CONTROLPANELWGT_H

#include <QWidget>
#include <QState>
#include <QStateMachine>
#include <QFinalState>


namespace Ui {
class ControlPanelWgt;
}

class ControlPanelWgt : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPanelWgt(QWidget *parent = nullptr);
    ~ControlPanelWgt();

    void changeButtons(QString newState);
    void setConnectionState(bool state);

signals:
    void btnStart();
    void btnStop();
    void btnPause();
    void btnContinue();

    void transitToConnect();
    void transitToDisconnect();
    void transitToIdle();
    void transitToProcess();
    void transitToPause();

private:


    Ui::ControlPanelWgt *ui;


    QState *startState = nullptr;
    QState *connectionState = nullptr;
    QState *disctonnectState = nullptr;
    QState *experimentState = nullptr;
    QState *idleState = nullptr;
    QState *pauseState = nullptr;

    QStateMachine *machine = nullptr;
};

#endif // CONTROLPANELWGT_H
