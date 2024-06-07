#include "operations.h"
#include "measurements.h"

using namespace Measurements;


#define DEBUG 1

Operations::Operations(quint8 addr) : Interface(addr)
{
    loadFrame.address = addr;
}

Operations::~Operations()
{
}


/* Не забыть сделать удаление jStatusOperation посе завершения
 */
bool Operations::execut()
{
    RETCODE ret;    // = next();
    switch(state) {
    case Operations::STATE_IDLE:
        ret = loadFrame.statusSensors(jStatusOperation);
        if (ret == COMPLATE) {
            counter++;
            if (counter % 10 == 0) {
                qDebug() << qPrintable(QString("force=%1(N)\t deform=%2(mm)\t stepperPos=%3\t stepperStatus=%4\t controllerStatus=%5\t counter=%6").
                        arg(Force::fromNewtons(loadFrame.forceSens.value).newtons(), 9).
                        arg(Length::fromMicrometres(loadFrame.deformSens.value).millimetres(), 9).
                        arg((loadFrame.stepper.position * 0.31250)/1000., 9).        // TODO 1:10 на эмуляторе
                        arg(loadFrame.stepper.status).
                        arg(loadFrame.controller.status).
                        arg(counter));
            }
            fflush(stderr);
        }
        break;
    case Operations::STATE_PROCESS:
        ret = execCMD(jStatusOperation);
        break;
    }


    if (ret != NOERROR) {
        resetCommunicationState();
//        qDebug() << "Читаем следующую команду из списка если она есть, меняем состояние на выполнение" << retcodeToStr(ret);
        read(jStatusOperation);
        if (jStatusOperation.isEmpty())
            state = STATE_IDLE;
        else
            state = STATE_PROCESS;
    } else {
        if (jStatusOperation["PDU_request"].toString().isEmpty())
            return true;
        write(jStatusOperation);    // send to modbas
    }
    return true;
}

RETCODE Operations::execCMD(QJsonObject &jobj)
{
//    qDebug().noquote() << Q_FUNC_INFO << jobj;
    if (jobj["CMD"].toString() == "settings") {
        QFile file;
        file.setFileName("test.txt");
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "file not open";
            return COMPLATE;
        }
        qDebug() << "write new config" << QJsonDocument(jobj["config"].toObject());
        file.write(QJsonDocument(jobj["config"].toObject()).toJson());
        file.close();
    } else if (jobj["CMD"].toString() == "get_sensor_value") {

    } else if (jobj["CMD"].toString() == "scan") {
        loadFrame.readSensors(jobj);
        emit sendRequestToClient(jobj);
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
        return loadFrame.moveFrame(jobj);
    } else if (jobj["CMD"].toString() == "stop_frame") {
        return loadFrame.stopFrame(jobj);
    } else if (jobj["CMD"].toString() == "unlock_PID") {
        loadFrame.unlockPID();
    }

    return COMPLATE;
}

RETCODE Operations::next()
{
    return loadFrame.next(jStatusOperation);
}

void Operations::resetCommunicationState()
{
    loadFrame.resetStateModeBusCommunication();
}
