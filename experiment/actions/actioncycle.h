#ifndef ACTIONCYCLE_H
#define ACTIONCYCLE_H

#include <QObject>

#include "baseaction.h"

class ActionCycle : public BaseAction
{
    Q_OBJECT

    enum STATE_PREP_DEVICE {
        STATE_PREP_LOAD_FRAME_1, STATE_PREP_LOAD_FRAME_2,
        STATE_PREP_VOLUMETER1_1, STATE_PREP_VOLUMETER1_2,
        STATE_PREP_VOLUMETER2_1, STATE_PREP_VOLUMETER2_2
    } state_prep_device = STATE_PREP_LOAD_FRAME_1;


    bool prepDevice();


public:
    explicit ActionCycle(QObject *parent = nullptr);
    ~ActionCycle();


    virtual bool updateSteping() = 0;
    virtual bool stepChanged() = 0;

    void init() override;
    bool update() override;
    void finishing() override;
    void pausing() override;


    // Работа с jActionCycle
    /**
     * @brief jIncCurStep           Устанавливает указатель на следующий шаг. Если такого шага не будет, то
     *                              возвращаем true, что вызовет finishing
     */
    void jIncCurStep();


    /**
     * @brief curStep               // Возвращает текущий Step
     * @return
     */
    QString curStep();

    QJsonObject jStep;
private:

    QTimer elapseTime;
    enum TRANS {
        TRANSITION_BEGIN,
        TRANSITION_READ_STEP,
        TRANSITION_PREP_DEVICE,
        TRANSITION_SET_STEP,
        TRANSITION_UPDATE_STEPING,
        TRANSITION_5,
        TRANSITION_FINISH,
    } trans = TRANSITION_BEGIN;

};

#endif // ACTIONCYCLE_H
