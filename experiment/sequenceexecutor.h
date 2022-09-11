#ifndef SEQUENCEEXECUTOR_H
#define SEQUENCEEXECUTOR_H

#include <QDebug>
#include <QJsonObject>
#include <QFile>
#include "operations.h"
#include "global.h"
#include "device.h"
//#include "actions/abstractaction.h"


class SequenceExecutor : public Operations
{

//    enum STATE_SEQUENCER {PROCESS, COMPLATE, ERROR} stateSequencer;
    LoadFrame loadFrame;

public:

//    QVector<AbstractAction *> actions;
    SequenceExecutor(quint8 addr);
    ~SequenceExecutor();

    void execCMD(QJsonObject &jobj) override;


private:
    void next() override;

};

#endif // SEQUENCEEXECUTOR_H
