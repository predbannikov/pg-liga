#include "operations.h"
#include "measurements.h"

using namespace Measurements;

Operations::Operations(quint8 addr) : Interface(addr),
    loadFrame(new LoadFrame),
    volumetr1(new Volumeter1),
    volumetr2(new Volumeter2),
    plata(0, ActBase)
{
    jConfig["area"] = "0.01";
    store = new Data(addr, jConfig);


    QVector<Sensor *> sens;
    sens.append(loadFrame->forceSens);
    sens.append(loadFrame->deformSens);
    sens.append(volumetr1->pressureSens);
    sens.append(volumetr2->pressureSens);

    store->setSensors(sens);
    store->setStepper(loadFrame->stepper);


    loadFrame->setStore(store);
    volumetr1->setStore(store);
    volumetr2->setStore(store);
    volumetr1->address = addr;
    volumetr2->address = addr;
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
                qDebug() << qPrintable(QString("force=%1(N)  deform=%2(mm)  LFPos=%3  LFStat=%4  LFCtrlStat=%5  "
                                               "Vol1pres=%6(Pa)  Vol1Pos=%7  Vol1Stat=%8  Vol1CtrlStat=%9 "
                                               "Vol2pres=%10(Pa)  Vol2Pos=%11  Vol1Stat=%12  Vol1CtrlStat=%13"
                                               " cntr=%14  szQueue=%15").
                                       arg(Force::fromNewtons(loadFrame->forceSens->value).newtons(), 6).
                                       arg(Length::fromMicrometres(loadFrame->deformSens->value).millimetres(), 5).
                                       //                        arg((loadFrame->stepper.position * 0.31250)/1000., 9).        // TODO 1:10 на эмуляторе
                                       arg(loadFrame->stepper->position).        // TODO 1:10 на эмуляторе
                                       arg(loadFrame->stepper->status).
                                       arg(loadFrame->controller->status).
                                       arg(Pressure::fromPascals(volumetr1->pressureSens->value).pascals(), 7).
                                       arg(volumetr1->stepper->position).        // TODO 1:10 на эмуляторе
                                       arg(volumetr1->stepper->status).
                                       arg(volumetr1->controller->status).
                                       arg(Pressure::fromPascals(volumetr2->pressureSens->value).pascals(), 7).
                                       arg(volumetr2->stepper->position).        // TODO 1:10 на эмуляторе
                                       arg(volumetr2->stepper->status).
                                       arg(volumetr2->controller->status).
                                       arg(counter).
                                       arg(queueRequest.size()));
            }
            // if (counter % 100 == 0) {
            //     store->exportToCSVofName("VerticalPressure_kPa", "VerticalDeform_mm");
            // }
            fflush(stderr);
            // Тут нужно запустить парсер нашего эксперимента
            // if (loadFrame->store != nullptr)
            //     loadFrame->store->updateData();
            // if (volumetr1->store != nullptr)
            //     volumetr1->store->updateData();
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
        QJsonObject jObj({{"LF_sensor_force", QString::number(loadFrame->forceSens->value)},
                          {"LF_sensor_deform", QString::number(loadFrame->deformSens->value)},
                          {"LF_sensor_hol", QString::number(loadFrame->stepper->status)},
                          {"LF_stepper_pos", QString::number(loadFrame->stepper->position)},
                          {"LF_controller_status", QString::number(loadFrame->controller->status)},
                          {"Vol1_sensor_pressure", QString::number(volumetr1->pressureSens->value)},
                          {"Vol1_sensor_hol", QString::number(volumetr1->stepper->status)},
                          {"Vol1_stepper_pos", QString::number(volumetr1->stepper->position)},
                          {"Vol1_controller_status", QString::number(volumetr1->controller->status)},
                          {"Vol2_sensor_pressure", QString::number(volumetr2->pressureSens->value)},
                          {"Vol2_sensor_hol", QString::number(volumetr2->stepper->status)},
                          {"Vol2_stepper_pos", QString::number(volumetr2->stepper->position)},
                          {"Vol2_controller_status", QString::number(volumetr2->controller->status)},
                         });
        jobj["sensors"] = jObj;
        jobj["status_experiment"] = getStateExperiment();
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "set_experiment") {
        jExperiment = jobj["experiment"].toObject();
    } else if (jobj["CMD"].toString() == "start_experiment") {
        if (stateExperiment == STATE_EXPERIMENT_IDLE)
            stateExperiment = STATE_EXPERIMENT_TRANSIT_TO_PROCESS;
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
//        loadFrame->sendProtocol(jobj);
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "enable_store_data") {
        jobj["msg"] = store->enableStoreData(true);
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "disable_store_data") {
        jobj["msg"] = store->enableStoreData(false);
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "init_store_data") {
        jobj["msg"] = store->initStoreData();
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "delete_store_data") {
        jobj["msg"] = store->deleteStoreData();
        emit sendRequestToClient(jobj);
    } else if (jobj["CMD"].toString() == "get_store_data") {
        if (store != nullptr) {
            store->sendStoreData(jobj);
        } else {
            jobj["store_data"] = QString(QByteArray("no experiment has been launched yet").toBase64());
        }
        emit sendRequestToClient(jobj);

    } else if (jobj["CMD"].toString() == "load_frame_move") {
        return loadFrame->moveFrame(jobj);
    } else if (jobj["CMD"].toString() == "load_frame_stop") {
        return loadFrame->stopFrame(jobj);
    } else if (jobj["CMD"].toString() == "load_frame_unlock_PID") {
        return loadFrame->unlockPID(jobj);
    } else if (jobj["CMD"].toString() == "load_frame_set_target") {
        return loadFrame->setTarget(jobj);
    } else if (jobj["CMD"].toString() == "load_frame_set_vibro") {
        return loadFrame->setTargetVibro(jobj);
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
     } else if (jobj["CMD"].toString() == "load_frame_stepper_set_zero") {
        return loadFrame->stepperSetNull(jobj);
    } else if (jobj["CMD"].toString() == "load_frame_sensor_set_zero") {
        return loadFrame->sensorSetZero(jobj);
    } else if (jobj["CMD"].toString() == "load_frame_sensor_reset_offset") {
        return loadFrame->resetSensorOffset(jobj);
    }
    else if (jobj["CMD"].toString() == "volumetr1_stepper_set_zero") {
        return volumetr1->stepperSetNull(jobj);
    } else if (jobj["CMD"].toString() == "volumetr1_move") {
        return volumetr1->movePiston(jobj);
    } else if (jobj["CMD"].toString() == "volumetr1_stop") {
        return volumetr1->stopPiston(jobj);
    } else if (jobj["CMD"].toString() == "volumetr1_sensor_set_zero") {
        return volumetr1->sensorSetZero(jobj);
    } else if (jobj["CMD"].toString() == "volumetr1_sensor_reset_offset") {
        return volumetr1->resetSensorOffset(jobj);
    } else if (jobj["CMD"].toString() == "volumetr1_unlock_PID") {
        return volumetr1->unlockPID(jobj);
    } else if (jobj["CMD"].toString() == "volumetr1_set_target") {
        return volumetr1->setTarget(jobj);
    }
    else if (jobj["CMD"].toString() == "volumetr2_move") {
        return volumetr2->movePiston(jobj);
    } else if (jobj["CMD"].toString() == "volumetr2_stop") {
        return volumetr2->stopPiston(jobj);
    } else if (jobj["CMD"].toString() == "volumetr2_stepper_set_zero") {
        return volumetr2->stepperSetNull(jobj);
    } else if (jobj["CMD"].toString() == "volumetr2_sensor_set_zero") {
        return volumetr2->sensorSetZero(jobj);
    } else if (jobj["CMD"].toString() == "volumetr2_sensor_reset_offset") {
        return volumetr2->resetSensorOffset(jobj);
    } else if (jobj["CMD"].toString() == "volumetr2_unlock_PID") {
        return volumetr2->unlockPID(jobj);
    } else if (jobj["CMD"].toString() == "volumetr2_set_target") {
        return volumetr2->setTarget(jobj);
    }

    return COMPLATE;
}

RETCODE Operations::readSensors(QJsonObject &jobj)
{
    switch (vol_stat) {
    case Operations::VOLUMETR2_SENSOR_PRESSURE:
        if (volumetr2->readSensorPressure(jobj) == COMPLATE)
            vol_stat = VOLUMETR2_HOL_STATUS;
        break;

    case Operations::VOLUMETR2_HOL_STATUS:
        if (volumetr2->readStatusHol(jobj) == COMPLATE)
            vol_stat = VOLUMETR2_POSITION;
        break;

    case Operations::VOLUMETR2_POSITION:
        if (volumetr2->readPosition(jobj) == COMPLATE)
            vol_stat = VOLUMETR2_CONTROLLER_STATUS;
        break;

    case Operations::VOLUMETR2_CONTROLLER_STATUS:
        if (volumetr2->readControllerStatus(jobj) == COMPLATE)
            vol_stat = VOLUMETR1_SENSOR_PRESSURE;
        break;

    case Operations::VOLUMETR1_SENSOR_PRESSURE:
        if (volumetr1->readSensorPressure(jobj) == COMPLATE)
            vol_stat = VOLUMETR1_HOL_STATUS;
        break;

    case Operations::VOLUMETR1_HOL_STATUS:
        if (volumetr1->readStatusHol(jobj) == COMPLATE)
            vol_stat = VOLUMETR1_POSITION;
        break;

    case Operations::VOLUMETR1_POSITION:
        if (volumetr1->readPosition(jobj) == COMPLATE)
            vol_stat = VOLUMETR1_CONTROLLER_STATUS;
        break;

    case Operations::VOLUMETR1_CONTROLLER_STATUS:
        if (volumetr1->readControllerStatus(jobj) == COMPLATE)
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
            vol_stat = VOLUMETR2_SENSOR_PRESSURE;
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
