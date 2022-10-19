#include "sequenceexecutor.h"


SequenceExecutor::SequenceExecutor(quint8 addr) : Operations(addr)
{
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
        loadFrame.readConfig();
        loadFrame.startProcess();
    } else if (jobj["CMD"].toString() == "read_config") {
        loadFrame.readConfig();
    } else if (jobj["CMD"].toString() == "read_sensors") {
        loadFrame.readSensors(jobj);
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "get_protocol") {
        loadFrame.sendProtocol(jobj);
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "get_store_data") {
        loadFrame.sendStoreData(jobj);
        loadFrame.readSensors(jobj);
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
