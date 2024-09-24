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
        trans = TRANSITION_BEGIN;
        state_prep_device = STATE_PREP_LOAD_FRAME_1;
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
                jStep = jOperation[curStep()].toObject();
                trans = TRANSITION_INIT_STEP;
                return false;
            }
        }
        sendError("Не найдено ни одного step, завершение операции", jOperation);
        trans = TRANSITION_FINISH;
        break;


    case ActionCycle::TRANSITION_INIT_STEP:
//        jStep = jOperation[curStep()].toObject();
        trans = TRANSITION_PREP_DEVICE;
        initStepping();
    case ActionCycle::TRANSITION_PREP_DEVICE:
        if (prepDevice())
            trans = TRANSITION_UPDATE_STEPING;
        break;

    case ActionCycle::TRANSITION_UPDATE_STEPING:
        if (updateSteping()) {

            // Завершаем цикл по дополнительному условию
            saveDevice("complate"); // Сохраняем данные и продолжаем цикл
            if (someAdditionalCondition()) {
                trans = TRANSITION_FINISH;
            } else {
                jIncCurStep();
                trans = TRANSITION_READ_STEP;
            }
            return false;
        }
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
        setMarkerTime(TIME_ELAPSE_FOR_ZERO_OPERATION);
        state_prep_device = STATE_PREP_LOAD_FRAME_2;
        break;
    case STATE_PREP_LOAD_FRAME_2:
        if (elapseMarkerTime() && loadFrame->stepper->position != 0) {
            sendError("Задержка больше 1000мс, по комманде load_frame_stepper_set_zero", jOperation);
            state_prep_device = STATE_PREP_LOAD_FRAME_1;
        }
        else if (loadFrame->stepper->position == 0) {
//            auto jObj = jOperation[curStep()].toObject();
//            jObj["LF_position"] = QString::number(loadFrame->stepper->position);
//            jObj["LF_force"] = QString::number(loadFrame->forceSens->value);
//            jObj["LF_deform"] = QString::number(loadFrame->deformSens->value);
//            updateJStep(jObj);
            state_prep_device = STATE_PREP_VOLUMETER1_1;
        }
        break;
    case STATE_PREP_VOLUMETER1_1:
        jCmdToQueue["CMD"] = "volumetr1_stepper_set_zero";
        putQueue(jCmdToQueue);
        setMarkerTime(TIME_ELAPSE_FOR_ZERO_OPERATION);
        state_prep_device = STATE_PREP_VOLUMETER1_2;
        break;
    case STATE_PREP_VOLUMETER1_2:
        if (elapseMarkerTime() && volumeter1->stepper->position != 0) {
            sendError("Задержка больше 1000мс, по комманде volumetr1_stepper_set_zero", jOperation);
            state_prep_device = STATE_PREP_VOLUMETER1_1;
        }
        else if (volumeter1->stepper->position == 0) {
//            auto jObj = jOperation[curStep()].toObject();
//            jObj["Vol1_position"] = QString::number(volumeter1->stepper->position);
//            jObj["Vol1_pressure"] = QString::number(volumeter1->pressureSens->value);
//            updateJStep(jObj);
            state_prep_device = STATE_PREP_VOLUMETER2_1;
        }
        break;
    case STATE_PREP_VOLUMETER2_1:
        jCmdToQueue["CMD"] = "volumetr2_stepper_set_zero";
        putQueue(jCmdToQueue);
        setMarkerTime(TIME_ELAPSE_FOR_ZERO_OPERATION);
        state_prep_device = STATE_PREP_VOLUMETER2_2;
        break;
    case STATE_PREP_VOLUMETER2_2:
        if (elapseMarkerTime() && volumeter2->stepper->position != 0) {
            sendError("Задержка больше 1000мс, по комманде volumetr2_stepper_set_zero", jOperation);
            state_prep_device = STATE_PREP_VOLUMETER1_1;
        }
        else if (volumeter2->stepper->position == 0) {
            saveDevice("begin");
//            auto jObj = jOperation[curStep()].toObject();
//            jObj["Vol2_position"] = QString::number(volumeter2->stepper->position);
//            jObj["Vol2_pressure"] = QString::number(volumeter2->pressureSens->value);
//            updateJStep(jObj);
            state_prep_device = STATE_PREP_LOAD_FRAME_1;
            return true;
        }
        break;

    }
    return false;
}

bool ActionCycle::saveDevice(const QString& prefix)
{
//    auto jStep = jOperation[curStep()].toObject();
    jStep["LF_position_" + prefix] = QString::number(loadFrame->stepper->position);
    jStep["LF_force_" + prefix] = QString::number(loadFrame->forceSens->value);
    jStep["LF_deform_" + prefix] = QString::number(loadFrame->deformSens->value);
    jStep["Vol1_position_" + prefix] = QString::number(volumeter1->stepper->position);
    jStep["Vol1_pressure_" + prefix] = QString::number(volumeter1->pressureSens->value);
    jStep["Vol2_position_" + prefix] = QString::number(volumeter2->stepper->position);
    jStep["Vol2_pressure_" + prefix] = QString::number(volumeter2->pressureSens->value);
    jOperation[curStep()] = jStep;
//    updateJStep(jStep);
    return true;
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
    qDebug().noquote() << QJsonDocument(jStep).toJson(QJsonDocument::Indented);

    jOperation[curStep()] = jStep;
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

QList<QJsonObject> ActionCycle::getJSteps()
{
    QMap <int, QJsonObject> map;
    for (QJsonObject::iterator iter = jOperation.begin(); iter != jOperation.end(); ++iter) {
        if (iter.key().contains("step")) {
            if (iter.value().toObject()["state"].toString() == "TRANS_FINISH_STEP")
                map.insert(iter.key().split('_')[1].toInt(), iter.value().toObject());
        }

    }
    QList<QJsonObject> list;
    for(auto it = map.begin(); it != map.end(); ++it)
        list.append(it.value());
    return list;
}

void ActionCycle::updateJStep(QJsonObject jObj)
{
    jOperation[curStep()] = jObj;
}
