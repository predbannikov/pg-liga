#include <QDebug>
#include "actioncycle.h"


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

}

bool ActionCycle::update()
{

    switch (trans) {
    case ActionCycle::TRANSITION_1:
        if (jAction.isEmpty())
            trans = TRANSITION_4;
        else {
            trans = TRANSITION_2;
        }
        break;

    case ActionCycle::TRANSITION_2:
        for (QJsonObject::iterator iter = jAction.begin(); iter != jAction.end(); ++iter) {
            if (iter.key() == curStep()) {
                qDebug() << "key=" << iter.key().split('_')[1] << "    curStep=" << curStep();
                trans = TRANSITION_3;
                return false;
            }
        }
        if (iter == jAction.end()) {
            sendError("Не найдено ни одного step, завершение операции", jAction);
            return true;
        }
        trans = TRANSITION_4;
        break;

    case ActionCycle::TRANSITION_3:
        if (updateSteping()) {
            jUpdateJAction(jStep);
            jIncCurStep();
            trans = TRANSITION_2;
            return false;
        }
        if (stepChanged())
            jUpdateJAction(jStep);
        break;

    case ActionCycle::TRANSITION_4:
        trans = TRANSITION_1;
        return true;    // Завершение цикла
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
    QJsonObject jActionStatus = jAction["status"].toObject();
    int curStep = jActionStatus["cur_step"].toString().split('_')[1].toInt();
    curStep++;
    jActionStatus["cur_step"] = QString("step_%1").arg(curStep);
    jAction["status"] = jActionStatus;

}

void ActionCycle::jUpdateJAction(QJsonObject jObj)
{

}

void ActionCycle::jSaveState(QString state)
{

}

QString ActionCycle::curStep()
{
    return jAction["status"].toObject()["cur_step"].toString();
}
