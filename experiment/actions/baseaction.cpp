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
    store->startOperation(jOperation);

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
