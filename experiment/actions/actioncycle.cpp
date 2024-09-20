#include <QDebug>
#include "actioncycle.h"

#include "loadframe.h"
#include "volumeter1.h"
#include "volumeter2.h"


ActionCycle::ActionCycle(QObject *parent)
    : BaseAction{parent}
{

}

ActionCycle::~ActionCycle()
{
    qDebug() << Q_FUNC_INFO;
}

void ActionCycle::init()
{
    if (jStatusOperation().contains("current_step")) {   // Уже была ранее запущена, нужно восстановить

    } else {        // Первый раз начинается операция
        QJsonObject jStatus = jStatusOperation();
        jStatus["current_step"] = "step_0";
        jSetStatusOperation(jStatus);
    }
}

bool ActionCycle::update()
{
    switch (trans) {

    case ActionCycle::TRANSITION_5:
        break;
    case ActionCycle::TRANSITION_BEGIN:
        if (jOperation.isEmpty())
            trans = TRANSITION_FINISH;
        else {
            trans = TRANSITION_READ_STEP;
        }
        break;

    case ActionCycle::TRANSITION_READ_STEP:
        for (QJsonObject::iterator iter = jOperation.begin(); iter != jOperation.end(); ++iter) {
            if (iter.key() == curStep()) {
                qDebug() << "key=" << iter.key().split('_')[1] << "    curStep=" << curStep();
                trans = TRANSITION_PREP_DEVICE;
                return false;
            }
        }
        sendError("Не найдено ни одного step, завершение операции", jOperation);
        trans = TRANSITION_FINISH;
        break;

    case ActionCycle::TRANSITION_PREP_DEVICE:
        if (prepDevice())
            trans = TRANSITION_SET_STEP;
        break;

    case ActionCycle::TRANSITION_SET_STEP:
        jStep = jOperation[curStep()].toObject();
        trans = TRANSITION_UPDATE_STEPING;
    case ActionCycle::TRANSITION_UPDATE_STEPING:
        if (updateSteping()) {
//            jUpdateStatusOperation(jStep);
            jIncCurStep();
            trans = TRANSITION_READ_STEP;
            return false;
        }
//        if (stepChanged())
//            jUpdateStatusOperation(jStep);
        break;

    case ActionCycle::TRANSITION_FINISH:
        trans = TRANSITION_BEGIN;
        return true;    // Завершение цикла
    }
    return false;
}

#define TIME_ELAPSE_FOR_ZERO_OPERATION  2000
bool ActionCycle::prepDevice()
{
    switch (state_prep_device) {
    case STATE_PREP_LOAD_FRAME_1:
        jCmdToQueue["CMD"] = "load_frame_stepper_set_zero";
        putQueue(jCmdToQueue);
        elapseTime.start(TIME_ELAPSE_FOR_ZERO_OPERATION);
        state_prep_device = STATE_PREP_LOAD_FRAME_2;
        break;
    case STATE_PREP_LOAD_FRAME_2:
        if (!elapseTime.isActive() && loadFrame->stepper->position != 0) {
            sendError("Задержка больше 1000мс, по комманде load_frame_stepper_set_zero", jOperation);
            state_prep_device = STATE_PREP_LOAD_FRAME_1;
        }
        else if (elapseTime.isActive() && loadFrame->stepper->position == 0) {
            auto jObj = jStatusOperation();
            jObj["LF_position"] = QString::number(0);
            jObj["LF_force"] = QString::number(loadFrame->forceSens->value);
            jObj["LF_deform"] = QString::number(loadFrame->deformSens->value);
            jSetStatusOperation(jObj);
            state_prep_device = STATE_PREP_VOLUMETER1_1;
        }
        break;
    case STATE_PREP_VOLUMETER1_1:
        jCmdToQueue["CMD"] = "volumetr1_stepper_set_zero";
        putQueue(jCmdToQueue);
        elapseTime.start(TIME_ELAPSE_FOR_ZERO_OPERATION);
        state_prep_device = STATE_PREP_VOLUMETER1_2;
        break;
    case STATE_PREP_VOLUMETER1_2:
        if (!elapseTime.isActive() && volumeter1->stepper->position != 0) {
            sendError("Задержка больше 1000мс, по комманде volumetr1_stepper_set_zero", jOperation);
            state_prep_device = STATE_PREP_VOLUMETER1_1;
        }
        else if (elapseTime.isActive() && volumeter1->stepper->position == 0) {
            auto jObj = jStatusOperation();
            jObj["Vol1_position"] = QString::number(0);
            jObj["Vol1_pressure"] = QString::number(volumeter1->pressureSens->value);
            jSetStatusOperation(jObj);
            state_prep_device = STATE_PREP_VOLUMETER2_1;
        }
        break;
    case STATE_PREP_VOLUMETER2_1:
        jCmdToQueue["CMD"] = "volumetr2_stepper_set_zero";
        putQueue(jCmdToQueue);
        elapseTime.start(TIME_ELAPSE_FOR_ZERO_OPERATION);
        state_prep_device = STATE_PREP_VOLUMETER2_2;
        break;
    case STATE_PREP_VOLUMETER2_2:
        if (!elapseTime.isActive() && volumeter2->stepper->position != 0) {
            sendError("Задержка больше 1000мс, по комманде volumetr2_stepper_set_zero", jOperation);
            state_prep_device = STATE_PREP_VOLUMETER1_1;
        }
        else if (elapseTime.isActive() && volumeter2->stepper->position == 0) {
            auto jObj = jStatusOperation();
            jObj["Vol2_position"] = QString::number(0);
            jObj["Vol2_pressure"] = QString::number(volumeter2->pressureSens->value);
            jSetStatusOperation(jObj);
            state_prep_device = STATE_PREP_LOAD_FRAME_1;
            return true;
        }
        break;

    }
    return false;
}


void ActionCycle::finishing()
{
    QJsonObject jobj;
    jobj["CMD"] = "load_frame_stop";
    putQueue(jobj);
}

void ActionCycle::pausing()
{
}

void ActionCycle::jIncCurStep()
{
    auto jStatus = jStatusOperation();
    int curStep = jStatus["current_step"].toString().split('_')[1].toInt();
    curStep++;
    jStatus["current_step"] = QString("step_%1").arg(curStep);
    jSetStatusOperation(jStatus);
}

//void ActionCycle::jUpdateStatusOperation(QJsonObject jObj)
//{
//    auto jObj = jOperation["status_operation"];
//}

//void ActionCycle::jSaveStatusOperation(QString state)
//{

//}

QString ActionCycle::curStep()
{
    return jOperation["status_operation"].toObject()["current_step"].toString();
}
