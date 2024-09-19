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
    case ActionCycle::TRANSITION_1:
        if (jOperation.isEmpty())
            trans = TRANSITION_4;
        else {
            trans = TRANSITION_2;
        }
        break;

    case ActionCycle::TRANSITION_2:
        for (QJsonObject::iterator iter = jOperation.begin(); iter != jOperation.end(); ++iter) {
            if (iter.key() == curStep()) {
                qDebug() << "key=" << iter.key().split('_')[1] << "    curStep=" << curStep();
                trans = TRANSITION_3;
                return false;
            }
        }
        sendError("Не найдено ни одного step, завершение операции", jOperation);
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
    auto jStatus = jStatusOperation();
    int curStep = jStatus["current_step"].toString().split('_')[1].toInt();
    curStep++;
    jStatus["current_step"] = QString("step_%1").arg(curStep);
    jSetStatusOperation(jStatus);
}

void ActionCycle::jUpdateJAction(QJsonObject jObj)
{

}

void ActionCycle::jSaveState(QString state)
{

}

QString ActionCycle::curStep()
{
    return jOperation["status_operation"].toObject()["current_step"].toString();
}
