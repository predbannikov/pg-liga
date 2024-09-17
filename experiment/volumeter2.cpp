#include "volumeter2.h"

Volumeter2::Volumeter2() :
    /// Волюмометр 2
    pressureSens(new Sensor(SensPress0Addr, SensPrs0)),
    stepper(new Stepper(ActVol0Addr, ActVol0)),
    controller(new Controller(CtrlVol0Addr, CtrlVol0))
{
    unitName = "Volumetr2";
    targetPressure = Measurements::Pressure::fromPascals(50.);
}

Volumeter2::~Volumeter2()
{
    delete pressureSens;
    delete stepper;
    delete controller;
    qDebug() << Q_FUNC_INFO;
}

bool Volumeter2::init()
{
    if (store != nullptr) {
        store->deleteLater();
        store = nullptr;
    }

    store = new StoreData(address, jconfig);

    if (store == nullptr)
        return false;

    QVector<Sensor *> sens;
    sens.append(pressureSens);

    store->setSensors(sens);
    store->setStepper(stepper);
    return true;
}

bool Volumeter2::deleteData()
{
    if (store != nullptr) {
        store->deleteLater();
        store = nullptr;
    }
    return true;
}

RETCODE Volumeter2::setTarget(QJsonObject &jOperation)
{
    RETCODE ret = ERROR;
    targetPressure = Measurements::Pressure::fromPascals(jOperation["target"].toDouble());
    ret = controller->setTarget(jOperation, targetPressure.pascals());      // 200.19642105368277
    return ret;
}

RETCODE Volumeter2::setKPID(QJsonObject &jOperation, AbstractUnit::CMD cmd)
{
    RETCODE ret = ERROR;
    float value = jOperation["value"].toDouble();
    ret = controller->setKPID(jOperation, value, cmd);
    return ret;
}

// RETCODE Volumeter2::setStatePid(QJsonObject &jOperation)
// {
//     RETCODE ret = ERROR;
//     STATE_SET_STATE_PID_CONTROLLER state_pid = STATE_SET_STATE_PID_CONTROLLER_DEFAULT;
//     if (jOperation["state_pid"].toString() == "P_correct") {
//         state_pid = STATE_P_CORRECTION;
//     } else if (jOperation["state_pid"].toString() == "controller_default") {
//         state_pid = STATE_SET_STATE_PID_CONTROLLER_DEFAULT;
//     }
//     ret = controller->setStatePidController(jOperation, state_pid);
//     return ret;
// }

RETCODE Volumeter2::statusSensors(QJsonObject &jOperation)
{
    RETCODE ret = ERROR;
    switch (readSensState) {
    case READ_SENS_1:
        ret = pressureSens->read(jOperation);
        if (ret == COMPLATE)
            readSensState = READ_SENS_3;
        break;

    case READ_SENS_2:
        ret = pressureSens->readRaw(jOperation);
        if (ret == COMPLATE)
            readSensState = READ_SENS_3;
        break;
    case READ_SENS_3:
        ret = stepper->readStatus(jOperation);
        if (ret == COMPLATE)
            readSensState = READ_SENS_4;
        break;
    case READ_SENS_4:
        ret = stepper->readPos(jOperation);
        if (ret == COMPLATE)
            readSensState = READ_CONTROLLER_STATUS;
        break;
    case READ_CONTROLLER_STATUS:
        ret = controller->readStatus(jOperation);
        if (ret == COMPLATE)
            readSensState = READ_SENS_6;
        break;
    case READ_SENS_6:
        ret = COMPLATE;
        readSensState = READ_SENS_1;
        if (store != nullptr)
            store->updateData();
        return COMPLATE;
        break;
    }
    if (ret == ERROR)
        return ERROR;
    return NOERROR;
}

void Volumeter2::resetStateModeBusCommunication()
{
    pressureSens->resetState();
    controller->resetState();
    stepper->resetState();
}

RETCODE Volumeter2::movePiston(QJsonObject &jobj)
{
    int speed = jobj["speed"].toString().toInt();
    return stepper->setSpeed(jobj, speed);
}

RETCODE Volumeter2::unlockPID(QJsonObject &jobj)
{
    return controller->stopPID(jobj);
}

RETCODE Volumeter2::stopPiston(QJsonObject &jobj)
{
    return stepper->stop(jobj);
}

RETCODE Volumeter2::sensorSetZero(QJsonObject &jobj)
{
    Sensor *sensor = getSensorFromStr(jobj["sensor_name"].toString());
    return sensor->setNull(jobj);
}

RETCODE Volumeter2::resetSensorOffset(QJsonObject &jobj)
{
    Sensor *sensor = getSensorFromStr(jobj["sensor_name"].toString());
    return sensor->resetOffset(jobj);
}

RETCODE Volumeter2::stepperSetNull(QJsonObject &jobj)
{
    return stepper->setNull(jobj);
}

void Volumeter2::readSensors(QJsonObject &jobj)
{
    QJsonObject jsensors;
    jsensors["pressure"] = QString::number(pressureSens->value);
    jsensors["stepper"] = QString::number(stepper->position);
    jobj["sensors"] = jsensors;
    jobj["status"] = "ok";
}

void Volumeter2::sendProtocol(QJsonObject &jobj)
{
    if (store != nullptr) {
        store->sendProtocol(jobj);
    } else {
        jobj["protocol"] = QString(QByteArray("no experiment has been launched yet").toBase64());
    }
}

void Volumeter2::sendStoreData(QJsonObject &jobj)
{
    if (store != nullptr) {
        store->sendStoreData(jobj);
    } else {
        jobj["store_data"] = QString(QByteArray("no experiment has been launched yet").toBase64());
    }
}

void Volumeter2::setStore(StoreData *pStore)
{
    store = pStore;
}

RETCODE Volumeter2::readSensorPressure(QJsonObject &jOperation)
{
    return pressureSens->read(jOperation);
}

RETCODE Volumeter2::readRawSensorPressure(QJsonObject &jOperation)
{
    return pressureSens->readRaw(jOperation);
}

RETCODE Volumeter2::readStatusHol(QJsonObject &jOperation)
{
    return stepper->readStatus(jOperation);
}

RETCODE Volumeter2::readPosition(QJsonObject &jOperation)
{
    return stepper->readPos(jOperation);
}

RETCODE Volumeter2::readControllerStatus(QJsonObject &jOperation)
{
    return controller->readStatus(jOperation);
}

RETCODE Volumeter2::updateStoreData(QJsonObject &jOperation)
{
    if (store != nullptr)
        store->updateData();
}
