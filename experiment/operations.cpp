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
//                        arg((loadFrame.stepper.position * 0.31250)/1000., 9).        // TODO 1:10 на эмуляторе
                        arg(loadFrame.stepper.position).        // TODO 1:10 на эмуляторе
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
        loadFrame.writeConfig(jobj);
        loadFrame.readConfig();
    } else if (jobj["CMD"].toString() == "get_sensor_value") {
        QString arg1 = jobj["arg1"].toString();
        double value = -1;
        if (arg1 == "SensLoad0") {
            value = loadFrame.forceSens.value;
        } else if (arg1 == "SensDef0") {
            value = loadFrame.deformSens.value;
        }
        jobj["value"] = QString::number(value);
        emit sendRequestToClient(jobj);
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
    } else if (jobj["CMD"].toString() == "init_store_data") {
        jobj["status"] = loadFrame.init();
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "get_store_data") {
        loadFrame.sendStoreData(jobj);
//        qDebug() << Q_FUNC_INFO << jobj;
        fflush(stderr);
//        loadFrame.readSensors(jobj);
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "stop_store_data") {
        loadFrame.deleteData();
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "move_frame") {
        return loadFrame.moveFrame(jobj);
    } else if (jobj["CMD"].toString() == "stop_frame") {
        return loadFrame.stopFrame(jobj);
    } else if (jobj["CMD"].toString() == "unlock_PID") {
        return loadFrame.unlockPID(jobj);
    } else if (jobj["CMD"].toString() == "set_target") {
        return loadFrame.setTarget(jobj);
    } else if (jobj["CMD"].toString() == "set_hz") {
        return loadFrame.setHz(jobj);
    } else if (jobj["CMD"].toString() == "hard_reset") {
        return loadFrame.hardReset(jobj);
    } else if (jobj["CMD"].toString() == "set_pid_p") {
        return loadFrame.setKPID(jobj, AbstractUnit::CMD::ControllerSetKp);
    } else if (jobj["CMD"].toString() == "set_pid_d") {
        return loadFrame.setKPID(jobj, AbstractUnit::CMD::ControllerSetKd);
    } else if (jobj["CMD"].toString() == "set_up_pid_p") {
        return loadFrame.setKPID(jobj, AbstractUnit::CMD::ControllerSetVibroKp);
    } else if (jobj["CMD"].toString() == "set_up_pid_d") {
        return loadFrame.setKPID(jobj, AbstractUnit::CMD::ControllerSetVibroKd);
    } else if (jobj["CMD"].toString() == "set_state_pid") {
        return loadFrame.setStatePid(jobj);
    } else if (jobj["CMD"].toString() == "set_sensor_zero") {
        return loadFrame.sensorSetZero(jobj);
    } else if (jobj["CMD"].toString() == "reset_sensor_offset") {
        return loadFrame.resetSensorOffset(jobj);
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
