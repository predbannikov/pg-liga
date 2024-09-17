#include "volumeter1.h"

Volumeter1::Volumeter1() :
    /// Волюмометр 1
    pressureSens(new Sensor(SensPress1Addr, SensPrs1)),
    stepper(new Stepper(ActVol1Addr, ActVol1)),
    controller(new Controller(CtrlVol1Addr, CtrlVol1))
{
    unitName = "Volumetr1";
    targetPressure = Measurements::Pressure::fromPascals(50.);
}

Volumeter1::~Volumeter1()
{
    delete pressureSens;
    delete stepper;
    delete controller;
    qDebug() << Q_FUNC_INFO;
}

bool Volumeter1::init()
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

bool Volumeter1::deleteData()
{
    if (store != nullptr) {
        store->deleteLater();
        store = nullptr;
    }
    return true;
}

RETCODE Volumeter1::setTarget(QJsonObject &jOperation)
{
    RETCODE ret = ERROR;
    targetPressure = Measurements::Pressure::fromPascals(jOperation["target"].toString().toDouble());
    ret = controller->setTarget(jOperation, targetPressure.pascals());      // 200.19642105368277
    return ret;
}

RETCODE Volumeter1::setKPID(QJsonObject &jOperation, AbstractUnit::CMD cmd)
{
    RETCODE ret = ERROR;
    float value = jOperation["value"].toDouble();
    ret = controller->setKPID(jOperation, value, cmd);
    return ret;
}

// RETCODE Volumeter1::setStatePid(QJsonObject &jOperation)
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

RETCODE Volumeter1::statusSensors(QJsonObject &jOperation)
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

void Volumeter1::resetStateModeBusCommunication()
{
    pressureSens->resetState();
    controller->resetState();
    stepper->resetState();
}

RETCODE Volumeter1::movePiston(QJsonObject &jobj)
{
    int speed = jobj["speed"].toString().toInt();
    return stepper->setSpeed(jobj, speed);
}

RETCODE Volumeter1::unlockPID(QJsonObject &jobj)
{
    return controller->stopPID(jobj);
}

RETCODE Volumeter1::stopPiston(QJsonObject &jobj)
{
    return stepper->stop(jobj);
}

RETCODE Volumeter1::sensorSetZero(QJsonObject &jobj)
{
    Sensor *sensor = getSensorFromStr(jobj["sensor_name"].toString());
    return sensor->setNull(jobj);
}

RETCODE Volumeter1::resetSensorOffset(QJsonObject &jobj)
{
    Sensor *sensor = getSensorFromStr(jobj["sensor_name"].toString());
    return sensor->resetOffset(jobj);
}

void Volumeter1::readSensors(QJsonObject &jobj)
{
    QJsonObject jsensors;
    jsensors["pressure"] = QString::number(pressureSens->value);
    jsensors["stepper"] = QString::number(stepper->position);
    jobj["sensors"] = jsensors;
    jobj["status"] = "ok";
}

void Volumeter1::sendProtocol(QJsonObject &jobj)
{
    if (store != nullptr) {
        store->sendProtocol(jobj);
    } else {
        jobj["protocol"] = QString(QByteArray("no experiment has been launched yet").toBase64());
    }
}

void Volumeter1::sendStoreData(QJsonObject &jobj)
{
    if (store != nullptr) {
        store->sendStoreData(jobj);
    } else {
        jobj["store_data"] = QString(QByteArray("no experiment has been launched yet").toBase64());
    }
}

void Volumeter1::setStore(StoreData *pStore)
{
    store = pStore;
}

RETCODE Volumeter1::readSensorPressure(QJsonObject &jOperation)
{
    return pressureSens->read(jOperation);
}

RETCODE Volumeter1::readRawSensorPressure(QJsonObject &jOperation)
{
    return pressureSens->readRaw(jOperation);
}

RETCODE Volumeter1::readStatusHol(QJsonObject &jOperation)
{
    return stepper->readStatus(jOperation);
}

RETCODE Volumeter1::readPosition(QJsonObject &jOperation)
{
    return stepper->readPos(jOperation);
}

RETCODE Volumeter1::readControllerStatus(QJsonObject &jOperation)
{
    return controller->readStatus(jOperation);
}

RETCODE Volumeter1::updateStoreData(QJsonObject &jOperation)
{
    if (store != nullptr)
        store->updateData();
}

RETCODE Volumeter1::stepperSetNull(QJsonObject &jobj)
{
    return stepper->setNull(jobj);
}
