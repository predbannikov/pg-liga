#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <QElapsedTimer>
#include <QTimer>
#include <QJsonObject>

#include "operations.h"



class Action : public QObject
{
    Q_OBJECT

    QElapsedTimer timElaps;

public:
    Action(QObject *parent = nullptr) : QObject(parent) {}
    ~Action(){}


    virtual bool update() = 0;
    virtual void init() = 0;
    virtual void finished() = 0;

    void initialization(QJsonObject jAct, LoadFrame *lf, Plata *plt) {
        loadFrame = lf;
        plata = plt;
        jAction = jAct;
        jAction["status"] = "process";
        init();
        Interface *interface = qobject_cast<Interface *>(parent());
        QObject::connect(this, &Action::sendCmd, interface, &Interface::put, Qt::QueuedConnection);

    }

    void finishing() {
        jAction["status"] = "complate";
        finished();
    }

    LoadFrame *loadFrame = nullptr;
    Plata *plata = nullptr;
    QJsonObject jAction;

signals:
    void sendCmd(QJsonObject &jobj);
};

class ActionMoveOfTime : public Action
{
    Q_OBJECT

    QTimer elapseTime;
    int speed = 0;
    enum TRANS {TRANS_1, TRANS_2} trans = TRANS_1;
public:
    ActionMoveOfTime(QObject *parent = nullptr) : Action(parent) {}
    ~ActionMoveOfTime() { qDebug() << Q_FUNC_INFO; }

    bool update() override {
        switch(trans) {
        case ActionMoveOfTime::TRANS_2:
            qDebug() << elapseTime.remainingTime();
            if (!elapseTime.isActive()) {
                qDebug() << "timer remaining";
                return true;
            }
            break;
        case ActionMoveOfTime::TRANS_1:
            QJsonObject jobj;
            jobj["CMD"] = "move_frame";
            jobj["speed"] = QString::number(speed);

            emit sendCmd(jobj);
            // qobject_cast<Interface *>(parent())->put(jobj);
            elapseTime.start();
            trans = TRANS_2;
            break;
        }
        return false;
    }
    void init() override {
        speed = jAction["speed"].toString().toInt();
        elapseTime.setSingleShot(true);
        elapseTime.setInterval(jAction["time_ms"].toString().toInt());
        qDebug() << "interval" << elapseTime.interval();
    }
    void finished() override {

    }
};


class Experiment : public Operations
{
    Q_OBJECT

    enum TRANSITION {TRANSITION_1, TRANSITION_2, TRANSITION_3, TRANSITION_4} transition = TRANSITION_1;

public:
    Experiment(quint8 addr);
    ~Experiment();
    void parser() override;

public slots:
    void doWork();


private:
    bool perform()
    {
        /** Мы можем читать датчики, но задавать целевые значения и передвижение
            шаговиков, необходимо выполнять через queueRequest (очередь команд)

            jExperiment содержит jActions
            нужно перебрать их все и когда все действия закончатся вернуть true
            что будет означать завершение всех действий

            нужно создать несколько кейсов
                1 кейс получаем номер акшиона
                2 кейс для выборки акшиона
                3 кейс для выполнения акшиона
                4 кейс для завершения эксперимента
        */

        switch (transition) {
        case Experiment::TRANSITION_1:
            if (jExperiment.isEmpty())
                transition = TRANSITION_4;
            else {
                curAction = jExperiment["curAction"].toString().toInt();
                transition = TRANSITION_2;
            }
            break;

        case Experiment::TRANSITION_2:
            for (QJsonObject::iterator iter = jExperiment.begin(); iter != jExperiment.end(); ++iter) {
                qDebug() << "key=" << iter.key() << "    curAction=" << curAction;
                if (iter.key().toInt() == curAction) {
                    action = new ActionMoveOfTime(this);
                    action->initialization(iter.value().toObject(), &loadFrame, &plata);
                    transition = TRANSITION_3;
                    return false;
                }
            }
            transition = TRANSITION_4;
            break;
        case Experiment::TRANSITION_3:
            if (action->update()) {
                action->finishing();
                jExperiment[QString::number(curAction)] = action->jAction;
                curAction++;
                delete action;
                transition = TRANSITION_2;
                return false;
            }
            break;

        case Experiment::TRANSITION_4:
            transition = TRANSITION_1;
            return true;
        }
        return false;
    }


    int curAction = 0;
    Action *action = nullptr;

};

#endif // EXPERIMENT_H
