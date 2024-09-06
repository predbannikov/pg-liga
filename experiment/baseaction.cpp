#include "baseaction.h"
#include "operations.h"

BaseAction::BaseAction(QObject *parent)
    : QObject{parent}
{

}

BaseAction::~BaseAction()
{

}

void BaseAction::initialization(QJsonObject jAct, LoadFrame *lf, Plata *plt) {
    loadFrame = lf;
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

void BaseAction::putQueue(QJsonObject &jObj) {
    qobject_cast<Operations *>(parent())->put(jObj);
}
