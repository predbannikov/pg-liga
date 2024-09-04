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

signals:
    void stopToFinal();
    void btnStart();
    void btnStop();
    void btnPause();
    void btnContinue();
//    void stateToWork();

private:


    Ui::ControlPanelWgt *ui;

    QState *workState = nullptr;
    QState *stopState = nullptr;
    QState *pauseState = nullptr;

    QStateMachine *machine = nullptr;
};

#endif // CONTROLPANELWGT_H
