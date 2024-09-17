#ifndef BASEACTION_H
#define BASEACTION_H

#include <QObject>
#include <QElapsedTimer>
#include <QTimer>
#include <QJsonObject>

class LoadFrame;
class Volumeter1;
class Volumeter2;
class Plata;

class BaseAction : public QObject
{
    Q_OBJECT

    QElapsedTimer timElaps;

    enum STATE_PAUSE {STATE_PAUSE_TRANSIT_1, STATE_PAUSE_TRANSIT_2} state_pause = STATE_PAUSE_TRANSIT_1;

public:
    explicit BaseAction(QObject *parent = nullptr);
    ~BaseAction();


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
    void initialization(QJsonObject jAct, LoadFrame *lf, Volumeter1 *vol1, Volumeter2 *vol2, Plata *plt);
    void finish();
    bool pause();
    void sendError(QString str, QJsonObject jobj = QJsonObject());


    bool jActionChanged() { return jChanged; }
    void putQueue(QJsonObject &jObj);

    LoadFrame *loadFrame = nullptr;
    Volumeter1 *volumeter1 = nullptr;
    Volumeter2 *volumeter2 = nullptr;
    Plata *plata = nullptr;
    QJsonObject jAction;

    /**
     * @brief jObj  временная переменная для отправки комманд в очередь
     */
    QJsonObject jCmdToQueue;
    int stepperPos = 0;
    bool jChanged = false;

signals:
    void error(QJsonObject);

};

#endif // BASEACTION_H
