#include "movebytimeloadframe.h"

#include <QDebug>

MoveByTimeLoadFrame::MoveByTimeLoadFrame(QObject *parent)
    : BaseAction{parent}
{

}

MoveByTimeLoadFrame::~MoveByTimeLoadFrame()
{
    qDebug() << Q_FUNC_INFO;
}

void MoveByTimeLoadFrame::init() {
    elapseTime.setSingleShot(true);
    elapseTime.setInterval(jOperation["time_ms"].toString().toInt());
}

bool MoveByTimeLoadFrame::update()
{
    switch(trans) {
    case TRANS_1:
        jCmdToQueue["CMD"] = "load_frame_move";
        jCmdToQueue["speed"] = jOperation["speed"].toString();
        putQueue(jCmdToQueue);
        elapseTime.start();
        trans = TRANS_2;
        break;

    case TRANS_2:
        if (!elapseTime.isActive()) {

            return true;
        }
        break;
    }
    return false;
}

void MoveByTimeLoadFrame::finishing()
{
    QJsonObject jobj;
    jobj["CMD"] = "load_frame_stop";
    putQueue(jobj);
}

void MoveByTimeLoadFrame::pausing()
{
}
