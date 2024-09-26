#ifndef BASEACTION_H
#define BASEACTION_H

#include <QObject>
#include <QElapsedTimer>
#include <QTimer>
#include <QJsonObject>

#include "data.h"

class LoadFrame;
class Volumeter1;
class Volumeter2;
class Plata;

class BaseAction : public QObject
{
    Q_OBJECT

    QElapsedTimer timElapsBase;
    QElapsedTimer elapseTimeBase;
    qint64 markerElapseTime;
    qint64 timerInterval;

    enum STATE_PAUSE {STATE_PAUSE_TRANSIT_1, STATE_PAUSE_TRANSIT_2} state_pause = STATE_PAUSE_TRANSIT_1;


    int loadFramePosition = 0;
    int volumeter1Position = 0;
    int volumeter2Position = 0;
    enum TRANSITION_TO_STOP_DEVICE {
        TRANSITION_TO_STOP_LOADFRAME_1, TRANSITION_TO_STOP_LOADFRAME_2,
        TRANSITION_TO_STOP_VOLUMETER1_1, TRANSITION_TO_STOP_VOLUMETER1_2,
        TRANSITION_TO_STOP_VOLUMETER2_1, TRANSITION_TO_STOP_VOLUMETER2_2,
    } transitionToStopDevice = TRANSITION_TO_STOP_LOADFRAME_1;

public:
    explicit BaseAction(QObject *parent = nullptr);
    ~BaseAction();
    void setMarkerTime(qint64 interval) {
        elapseTimeBase.start();
        markerElapseTime = elapseTimeBase.elapsed();
        timerInterval = interval;
    }
    bool elapseMarkerTime() {
        return elapseTimeBase.elapsed() - markerElapseTime > timerInterval;
    }



    /**
     * @brief update        Для выполнения комманд задать jCmdToQueue джейсонку в соответствии с апишкой
     *                      далее её задать через putQueueв очередь выполнения комманд
     * @return
     */

    virtual bool update() = 0;
    virtual void init() = 0;
    virtual void finishing() = 0;
    virtual void pausing() = 0;


    bool updating();
    void initialization(QJsonObject jAct, LoadFrame *lf, Volumeter1 *vol1, Volumeter2 *vol2, Plata *plt, Data *data);
    void finish();
    bool pause();
    void sendError(QString str, QJsonObject jobj = QJsonObject());


    bool jActionChanged() { return jChanged; }
    QJsonObject jStatusOperation() { return jOperation["status_operation"].toObject(); }
    void jSetStatusOperation(QJsonObject jObj) { jOperation["status_operation"] = jObj; }

    void putQueue(QJsonObject &jObj);

    LoadFrame *loadFrame = nullptr;
    Volumeter1 *volumeter1 = nullptr;
    Volumeter2 *volumeter2 = nullptr;
    Plata *plata = nullptr;
    Data *store = nullptr;
    QJsonObject jOperation;

    /**
     * @brief jObj  временная переменная для отправки комманд в очередь
     */
    QJsonObject jCmdToQueue;
    int stepperPos = 0;
    bool jChanged = false;

    bool stopDevice();
signals:
    void error(QJsonObject);

};

#endif // BASEACTION_H
