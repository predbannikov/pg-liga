#include "steppedpressure.h"

#include <QDebug>

#include "volumeter1.h"

SteppedPressure::SteppedPressure(QObject *parent)
    : BaseAction{parent}
{

}

SteppedPressure::~SteppedPressure()
{
    qDebug() << Q_FUNC_INFO;
}

void SteppedPressure::init() {
    elapseTime.setSingleShot(true);
    elapseTime.setInterval(1000);
    curStep = jAction["curStep"].toString().toInt();
}

bool SteppedPressure::update()
{
    switch(trans) {

    case TRANS_1: {
        QJsonObject::iterator iter;
        for (iter = jAction.begin(); iter != jAction.end(); ++iter) {
            if (iter.key().contains("step") && iter.key().split('_')[1].toInt() == curStep) {
                qDebug() << "key=" << iter.key().split('_')[1] << "    curStep=" << curStep;
                trans = TRANS_2;
                return false;
            }
        }
        if (iter == jAction.end()) {
            sendError("Не найдено ни одного step", jAction);
            return true;
        }
    }
        break;

    case TRANS_2:
        jCmdToQueue["CMD"] = "volumetr1_stepper_set_zero";
        putQueue(jCmdToQueue);
        elapseTime.start(1000);
        trans = TRANS_3;
        break;

    case SteppedPressure::TRANS_3:
        if (!elapseTime.isActive() && volumeter1->stepper->position != 0) {
            sendError("Задержка больше 1000мс, по комманде volumetr1_stepper_set_zero", jAction);
            trans = TRANS_2;
        }
        else if (elapseTime.isActive() && volumeter1->stepper->position == 0)
            trans = TRANS_4;
        break;

    case SteppedPressure::TRANS_4:
        jCmdToQueue["CMD"] = "volumetr1_set_target";
        jCmdToQueue["target"] = jAction[QString("step_%1").arg(curStep)].toObject()["target"].toString();
        putQueue(jCmdToQueue);
        elapseTime.start(1000);
        trans = TRANS_5;
        break;

    case SteppedPressure::TRANS_5:
        if (!elapseTime.isActive() && volumeter1->controller->status == 0) {
            sendError("Задержка больше 1000мс, по комманде volumetr1_set_target", jAction);
            trans = TRANS_4;
        } else if (elapseTime.isActive() && volumeter1->controller->status != 0) {
            trans = TRANS_6;
        }
        break;

    case SteppedPressure::TRANS_6:
        break;

    case SteppedPressure::TRANS_7:
        break;

    }
    return false;
}

void SteppedPressure::finishing()
{
}

void SteppedPressure::pausing()
{
}
