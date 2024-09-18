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

void BaseAction::initialization(QJsonObject jAct, LoadFrame *lf, Volumeter1 *vol1, Volumeter2 *vol2, Plata *plt) {
    loadFrame = lf;
    volumeter1 = vol1;
    volumeter2 = vol2;
    plata = plt;
    jAction = jAct;
    jAction["status"] = "process";
    init();
}

void BaseAction::finish() {
    jAction["status"] = "complate";
    finishing();
}

bool BaseAction::pause()
{
    jAction["status"] = "paused";
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
