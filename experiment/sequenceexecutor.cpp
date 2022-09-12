#include "sequenceexecutor.h"
#include <QtEndian>
#include <QDataStream>
//#include "actions/actiongetid.h"
//#include "actions/actiontest.h"
//#include "actions/actionstepperspeed.h"
//#include "actions/actionstepperstop.h"


SequenceExecutor::SequenceExecutor(quint8 addr) : Operations(addr)
{
//    actions.push_back(new ActionGetID);
//    actions.push_back(new ActionTest);
//    actions.push_back(new ActionStepperSpeed(0xAAAB4512, 0x0A, 0x2));
//    actions.push_back(new ActionStepperStop(0x2));
//    actions.push_back(new ActionStepperSpeed(0xAAABC5A6, 0x0A, 0x2));
//    actions.push_back(new ActionStepperStop(0x2));
    loadFrame.address = addr;
}

SequenceExecutor::~SequenceExecutor()
{
    qDebug() << Q_FUNC_INFO;
}

void SequenceExecutor::execCMD(QJsonObject &jobj)
{
    if (jobj["CMD"].toString() == "settings") {
        QFile file;
        file.setFileName("test.txt");
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "file not open";
            return;
        }
        qDebug() << "write new config" << QJsonDocument(jobj["config"].toObject());
        file.write(QJsonDocument(jobj["config"].toObject()).toJson());
        file.close();
    } else if (jobj["CMD"].toString() == "start") {
        loadFrame.startProcess();
    } else if (jobj["CMD"].toString() == "read_config") {
        loadFrame.readConfig();
    } else if (jobj["CMD"].toString() == "read_sensors") {
        loadFrame.readSensors(jobj);
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "get_protocol") {
        loadFrame.sendProtocol(jobj);
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "move_frame") {
        loadFrame.moveFrame(jobj);
    } else if (jobj["CMD"].toString() == "stop_frame") {
        loadFrame.stopFrame();
    } else if (jobj["CMD"].toString() == "unlock_PID") {
        loadFrame.unlockPID();
    }
}

void SequenceExecutor::next()
{

    RETCODE ret = loadFrame.next(statusOperation);
    if (ret == COMPLATE) {
        qDebug() << "succ, stop";
        stop();
    } else if (ret == ERROR) {
        qDebug() << "error";
        stop();
    }
//    if (current_index < actions.size()) {
//        if (actions[current_index]->next(statusOperation) == PROCESS_COMPLATE) {
//            qDebug() << "action success";
//            QThread::sleep(1);
//            current_index++;
//        }
//    } else {
//        // TODO Если последовательность выполнена, загружаем следующую с эксперимента
//        stop();

//    }

}
