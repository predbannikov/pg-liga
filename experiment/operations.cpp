#include "operations.h"
#include "measurements.h"

using namespace Measurements;


Operations::Operations(quint8 addr) : Interface(addr),
    loadFrame(new LoadFrame),
    plata(0, ActBase)
{
    jConfig["area"] = "0.01";
    store = new StoreData(addr, jConfig);


    QVector<Sensor *> sens;
    sens.append(loadFrame->forceSens);
    sens.append(loadFrame->deformSens);
    sens.append(volumetr1.pressureSens);

    store->setSensors(sens);
    store->setStepper(loadFrame->stepper);


    loadFrame->setStore(store);
    volumetr1.address = addr;
    loadFrame->address = addr;
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
        ret = readSensors(jStatusOperation);
        if (ret == COMPLATE) {
            counter++;
            if (counter % 10 == 0) {
                qDebug() << qPrintable(QString("force=%1(N)  deform=%2(mm)  LFPos=%3  LFStatus=%4  LFCtrlStatus=%5  "
                                               "Vol1pres=%6(Pa)  Vol1Pos=%7  Vol1Status=%8  Vol1CtrlStatus=%9 cntr=%10  szQueue=%11").
                                       arg(Force::fromNewtons(loadFrame->forceSens->value).newtons(), 9).
                                       arg(Length::fromMicrometres(loadFrame->deformSens->value).millimetres(), 9).
                                       //                        arg((loadFrame->stepper.position * 0.31250)/1000., 9).        // TODO 1:10 на эмуляторе
                                       arg(loadFrame->stepper->position).        // TODO 1:10 на эмуляторе
                                       arg(loadFrame->stepper->status).
                                       arg(loadFrame->controller->status).
                                       arg(Pressure::fromPascals(volumetr1.pressureSens->value).pascals(), 9).
                                       arg(volumetr1.stepper->position).        // TODO 1:10 на эмуляторе
                                       arg(volumetr1.stepper->status).
                                       arg(volumetr1.controller->status).
                                       arg(counter).
                                       arg(queueRequest.size()));
            }
            fflush(stderr);
            // Тут нужно запустить парсер нашего эксперимента
            // if (loadFrame->store != nullptr)
            //     loadFrame->store->updateData();
            // if (volumetr1.store != nullptr)
            //     volumetr1.store->updateData();
            store->updateData();
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
        QJsonObject jObj({{"Сила", loadFrame->forceSens->value},
                          {"Деформация", loadFrame->deformSens->value},
                          {"Давление1", volumetr1.pressureSens->value}
                         });
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
        loadFrame->readSensors(jobj);
        emit sendRequestToClient(jobj);
     } else if (jobj["CMD"].toString() == "read_sensors") {
        loadFrame->readSensors(jobj);
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "get_protocol") {
        loadFrame->sendProtocol(jobj);
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "init_store_data") {
        jobj["status"] = loadFrame->init();
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "get_store_data") {
        loadFrame->sendStoreData(jobj);
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "stop_store_data") {
        loadFrame->deleteData();
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "move_frame") {
        return loadFrame->moveFrame(jobj);
    } else if (jobj["CMD"].toString() == "stop_frame") {
        return loadFrame->stopFrame(jobj);
    } else if (jobj["CMD"].toString() == "unlock_PID") {
        return loadFrame->unlockPID(jobj);
    } else if (jobj["CMD"].toString() == "volumetr1_unlock_PID") {
        return volumetr1.unlockPID(jobj);
    } else if (jobj["CMD"].toString() == "volumetr1_set_target") {
        return volumetr1.setTarget(jobj);
    } else if (jobj["CMD"].toString() == "set_target") {
        return loadFrame->setTarget(jobj);
    } else if (jobj["CMD"].toString() == "set_hz") {
        return loadFrame->setHz(jobj);
    } else if (jobj["CMD"].toString() == "hard_reset") {
        return plata.write(jobj, HardReset);
    } else if (jobj["CMD"].toString() == "set_pid_p") {
        return loadFrame->setKPID(jobj, AbstractUnit::CMD::ControllerSetKp);
    } else if (jobj["CMD"].toString() == "set_pid_d") {
        return loadFrame->setKPID(jobj, AbstractUnit::CMD::ControllerSetKd);
    } else if (jobj["CMD"].toString() == "set_up_pid_p") {
        return loadFrame->setKPID(jobj, AbstractUnit::CMD::ControllerSetVibroKp);
    } else if (jobj["CMD"].toString() == "set_up_pid_d") {
        return loadFrame->setKPID(jobj, AbstractUnit::CMD::ControllerSetVibroKd);
    } else if (jobj["CMD"].toString() == "set_state_pid") {
        // return loadFrame->setStatePid(jobj);
    } else if (jobj["CMD"].toString() == "load_frame_sensor_set_zero") {
        return loadFrame->sensorSetZero(jobj);
    } else if (jobj["CMD"].toString() == "load_frame_sensor_reset_offset") {
        return loadFrame->resetSensorOffset(jobj);
    } else if (jobj["CMD"].toString() == "volumetr1_sensor_set_zero") {
        return volumetr1.sensorSetZero(jobj);
    } else if (jobj["CMD"].toString() == "volumetr1_sensor_reset_offset") {
        return volumetr1.resetSensorOffset(jobj);
    }

    return COMPLATE;
}

RETCODE Operations::readSensors(QJsonObject &jobj)
{
    switch (vol_stat) {

    case Operations::VOLUMETR1_SENSOR_PRESSURE:
        if (volumetr1.readSensorPressure(jobj) == COMPLATE)
            vol_stat = VOLUMETR1_HOL_STATUS;
        break;
    case Operations::VOLUMETR1_HOL_STATUS:
        if (volumetr1.readStatusHol(jobj) == COMPLATE)
            vol_stat = VOLUMETR1_POSITION;
        break;
    case Operations::VOLUMETR1_POSITION:
        if (volumetr1.readPosition(jobj) == COMPLATE)
            vol_stat = VOLUMETR1_CONTROLLER_STATUS;
        break;
    case Operations::VOLUMETR1_CONTROLLER_STATUS:
        if (volumetr1.readControllerStatus(jobj) == COMPLATE)
            vol_stat = LOADFRAME_SENSOR_FORCE;
        break;
    case Operations::LOADFRAME_SENSOR_FORCE:
        if (loadFrame->readSensorForce(jobj) == COMPLATE)
            vol_stat = LOADFRAME_SENSOR_DEFORM;
        break;
    case Operations::LOADFRAME_SENSOR_DEFORM:
        if (loadFrame->readSensorDeform(jobj) == COMPLATE)
            vol_stat = LOADFRAME_HOL_STATUS;
        break;
    case Operations::LOADFRAME_HOL_STATUS:
        if (loadFrame->readHolStatus(jobj) == COMPLATE)
            vol_stat = LOADFRAME_POSITION;
        break;
    case Operations::LOADFRAME_POSITION:
        if (loadFrame->readPosition(jobj) == COMPLATE)
            vol_stat = LOADFRAME_CONTROLLER_STATUS;
        break;
    case Operations::LOADFRAME_CONTROLLER_STATUS:
        if (loadFrame->readControllerStatus(jobj) == COMPLATE) {
            vol_stat = VOLUMETR1_SENSOR_PRESSURE;
            return COMPLATE;
        }
        break;
    }
    return NOERROR;
    // for (int i = 0; i < sizeof(vol_stat) * 8; ++i)
    //     if ((vol_stat & (1u << i)) != 0)
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
    loadFrame->resetStateModeBusCommunication();
}
