#include "operations.h"
#include "measurements.h"

using namespace Measurements;


Operations::Operations(quint8 addr) : Interface(addr),
    plata(0, ActBase)
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
    RETCODE ret;    // Переключает состояния если возврат не равно NOERROR
    switch(state_mode) {
    case Operations::STATE_MODE_IDLE:
        ret = loadFrame.statusSensors(jStatusOperation);
        if (ret == COMPLATE) {
            counter++;
            if (counter % 1 == 0) {
                qDebug() << qPrintable(QString("force=%1(N)\t deform=%2(mm)\t stepperPos=%3\t stepperStatus=%4\t controllerStatus=%5\t counter=%6\t sizeQueue=%7 ").
                        arg(Force::fromNewtons(loadFrame.forceSens->value).newtons(), 9).
                        arg(Length::fromMicrometres(loadFrame.deformSens->value).millimetres(), 9).
//                        arg((loadFrame.stepper.position * 0.31250)/1000., 9).        // TODO 1:10 на эмуляторе
                        arg(loadFrame.stepper->position).        // TODO 1:10 на эмуляторе
                        arg(loadFrame.stepper->status).
                        arg(loadFrame.controller->status).
                        arg(counter).
                        arg(queueRequest.size()));
            }
            fflush(stderr);
            // Тут нужно запустить парсер нашего эксперимента
            stateSwitch();
        }
        break;
    case Operations::STATE_MODE_EXECCMD:
        ret = execCMD(jStatusOperation);
        if (ret == COMPLATE) {
            // qDebug() << jStatusOperation;
            state_mode = STATE_MODE_IDLE;
            return true;
        }
        break;
    }


    if (ret != NOERROR) {
        resetCommunicationState();
//        qDebug() << "Читаем следующую команду из списка если она есть, меняем состояние на выполнение" << retcodeToStr(ret);
        read(jStatusOperation);
        if (jStatusOperation.isEmpty())
            state_mode = STATE_MODE_IDLE;
        else
            state_mode = STATE_MODE_EXECCMD;
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
    if (jobj["CMD"].toString() == "get_status_device") {
        QJsonObject jObj({{"Сила", loadFrame.forceSens->value},
                          {"Деформация", loadFrame.deformSens->value}});
        jobj["sensors"] = jObj;
        jobj["status_experiment"] = getStateExperiment();
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "set_experiment") {
        jExperiment = jobj["experiment"].toObject();
    } else if (jobj["CMD"].toString() == "start_experiment") {
        if (stateExperiment == STATE_EXPERIMENT_IDLE)
            stateExperiment = STATE_EXPERIMENT_PROCESS;
    } else if (jobj["CMD"].toString() == "stop_experiment") {
        if (stateExperiment == STATE_EXPERIMENT_PROCESS || stateExperiment == STATE_EXPERIMENT_PAUSE)
            stateExperiment = STATE_EXPERIMENT_TRANSIT_TO_STOP;
    } else if (jobj["CMD"].toString() == "continue_experiment") {
        if (stateExperiment == STATE_EXPERIMENT_PAUSE)
            stateExperiment = STATE_EXPERIMENT_PROCESS;
    } else if (jobj["CMD"].toString() == "pause_experiment") {
        if (stateExperiment == STATE_EXPERIMENT_PROCESS)
            stateExperiment = STATE_EXPERIMENT_TRANSIT_TO_PAUSE;
    } else if (jobj["CMD"].toString() == "scan") {
        loadFrame.readSensors(jobj);
        emit sendRequestToClient(jobj);
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
        return plata.write(jobj, HardReset);
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

QString Operations::getStateExperiment()
{
    switch(stateExperiment) {
    case Operations::STATE_EXPERIMENT_TRANSIT_TO_PAUSE:
        return QString("STATE_EXPERIMENT_TRANSIT_TO_PAUSE");
    case Operations::STATE_EXPERIMENT_TRANSIT_TO_STOP:
        return QString("STATE_EXPERIMENT_TRANSIT_TO_STOP");
    case Operations::STATE_EXPERIMENT_IDLE:
        return QString("STATE_EXPERIMENT_IDLE");
    case Operations::STATE_EXPERIMENT_PROCESS:
        return QString("STATE_EXPERIMENT_PROCESS");
    case Operations::STATE_EXPERIMENT_PAUSE:
        return QString("STATE_EXPERIMENT_PAUSE");
    }
    return QString("STATE_UNKNOWN");
}

void Operations::resetCommunicationState()
{
    loadFrame.resetStateModeBusCommunication();
}
