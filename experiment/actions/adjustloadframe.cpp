#include <QElapsedTimer>

#include "adjustloadframe.h"

AdjustByForceLoadFrame::AdjustByForceLoadFrame(QObject *parent)
    : BaseAction{parent}
{

}

AdjustByForceLoadFrame::~AdjustByForceLoadFrame()
{
    qDebug() << Q_FUNC_INFO;
}

void AdjustByForceLoadFrame::init() {
    elapseTime.setSingleShot(true);
    elapseTime.setInterval(jAction["time_ms"].toString().toInt());
}

bool AdjustByForceLoadFrame::update()
{
    switch(trans) {
    case TRANS_1: {
        QJsonObject jobj;
        jobj["CMD"] = "move_frame";
        jobj["speed"] = jAction["speed"].toString();
        putQueue(jobj);
        elapseTime.start();
        trans = TRANS_2;
        break;
    }
    case TRANS_2: {
        if (!elapseTime.isActive()) {

            return true;
        }
        break;
    }
    }
    return false;
}

void AdjustByForceLoadFrame::finishing()
{
    QJsonObject jobj;
    jobj["CMD"] = "stop_frame";
    putQueue(jobj);
}
