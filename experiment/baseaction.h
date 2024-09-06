#ifndef BASEACTION_H
#define BASEACTION_H

#include <QObject>
#include <QElapsedTimer>
#include <QTimer>
#include <QJsonObject>

class LoadFrame;
class Plata;

class BaseAction : public QObject
{
    Q_OBJECT

    QElapsedTimer timElaps;
public:
    explicit BaseAction(QObject *parent = nullptr);
    ~BaseAction();


    virtual bool update() = 0;
    virtual void init() = 0;
    virtual void finishing() = 0;
    virtual void pausing() = 0;

    void initialization(QJsonObject jAct, LoadFrame *lf, Plata *plt);
    void finish();
    bool pause();


    void putQueue(QJsonObject &jObj);

    LoadFrame *loadFrame = nullptr;
    Plata *plata = nullptr;
    QJsonObject jAction;

};

#endif // BASEACTION_H
