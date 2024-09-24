#include "baseaction.h"
#include "operations.h"


BaseAction::BaseAction(QObject *parent)
    : QObject{parent}
{

}

BaseAction::~BaseAction()
{

}

bool BaseAction::updating()
{
    jCmdToQueue = QJsonObject();
    return update();
}

void BaseAction::initialization(QJsonObject jAct, LoadFrame *lf, Volumeter1 *vol1, Volumeter2 *vol2, Plata *plt, Data *data) {
    loadFrame = lf;
    volumeter1 = vol1;
    volumeter2 = vol2;
    plata = plt;
    store = data;
    jOperation = jAct;

    if (jOperation.contains("status_operation")) {
        if (jOperation["status_operation"].toObject()["state"].toString() == "process") {
            // Восстанавливаем операцию
        }
        // доделать другие состояния

    // Запускаем новую операцию
    } else {
        QJsonObject jStatus = jStatusOperation();
        jStatus["state"] = "process";
        jSetStatusOperation(jStatus);
        store->startOperation(jOperation);
    }
    init();
}

void BaseAction::finish() {
    QJsonObject jStatus = jStatusOperation();
    jStatus["state"] = "complate";
    jSetStatusOperation(jStatus);
    store->stopOperation(jOperation);

    finishing();
}

bool BaseAction::pause()
{
    QJsonObject jStatus = jStatusOperation();
    jStatus["state"] = "paused";
    jSetStatusOperation(jStatus);
    finishing();
    return true;
}

void BaseAction::sendError(QString str, QJsonObject jobj)
{
    jCmdToQueue["error"] = str;
    jCmdToQueue["jObject"] = jobj;
    emit error(jCmdToQueue);
}

void BaseAction::putQueue(QJsonObject &jObj) {
    qobject_cast<Operations *>(parent())->put(jObj);
}

bool BaseAction::stopDevice()
{
    QJsonObject jobj;

    switch (transitionToStopDevice) {

    case TRANSITION_TO_STOP_LOADFRAME_1:
         jobj["CMD"] = "load_frame_unlock_PID";
        putQueue(jobj);
        jobj["CMD"] = "load_frame_stop";
        putQueue(jobj);
        transitionToStopDevice = TRANSITION_TO_STOP_LOADFRAME_2;
        loadFramePosition = loadFrame->stepper->position;
        break;
    case TRANSITION_TO_STOP_LOADFRAME_2:
        if (loadFrame->controller->status == 0 && loadFramePosition == loadFrame->stepper->position) {
            transitionToStopDevice = TRANSITION_TO_STOP_VOLUMETER1_1;
        }
        loadFramePosition = loadFrame->stepper->position;
        break;
    case TRANSITION_TO_STOP_VOLUMETER1_1:
        jobj["CMD"] = "volumetr1_unlock_PID";
        putQueue(jobj);
        jobj["CMD"] = "volumetr1_stop";
        putQueue(jobj);
        transitionToStopDevice = TRANSITION_TO_STOP_VOLUMETER1_2;
        volumeter1Position = volumeter1->stepper->position;
        break;
    case TRANSITION_TO_STOP_VOLUMETER1_2:
        if (volumeter1->controller->status == 0 && volumeter1Position == volumeter1->stepper->position) {
            transitionToStopDevice = TRANSITION_TO_STOP_VOLUMETER2_1;
        }
        volumeter1Position = volumeter1->stepper->position;
        break;
    case TRANSITION_TO_STOP_VOLUMETER2_1:
        jobj["CMD"] = "volumetr2_unlock_PID";
        putQueue(jobj);
        jobj["CMD"] = "volumetr2_stop";
        putQueue(jobj);
        transitionToStopDevice = TRANSITION_TO_STOP_VOLUMETER2_2;
        volumeter2Position = volumeter2->stepper->position;
        break;
    case TRANSITION_TO_STOP_VOLUMETER2_2:
        if (volumeter2->controller->status == 0 && volumeter2Position == volumeter2->stepper->position) {
            transitionToStopDevice = TRANSITION_TO_STOP_LOADFRAME_1;
            return true;
        }
        volumeter2Position = volumeter2->stepper->position;
        break;
    }
    return false;
}
