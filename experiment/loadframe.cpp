#include "loadframe.h"

LoadFrame::LoadFrame() :
    /// первая рама
    forceSens(new Sensor(SensLoad0Addr, SensLoad0)),
    deformSens(new Sensor(SensDeform0Addr, SensDef0)),
    stepper(new Stepper(ActLoad0Addr, ActLoad0)),
    controller(new Controller(CtrlLoad0Addr, CtrlLoad0))
{
    targetNewtones = Measurements::Force::fromNewtons(50.);
}

LoadFrame::~LoadFrame()
{
    delete forceSens;
    delete deformSens;
    delete stepper;
    delete controller;
    qDebug() << Q_FUNC_INFO;
}

bool LoadFrame::init()
{
    if (store != nullptr) {
        store->deleteLater();
        store = nullptr;
    }

    store = new StoreData(address, jconfig);

    if (store == nullptr)
        return false;

    QVector<Sensor *> sens;
    sens.append(forceSens);
    sens.append(deformSens);

    store->setSensors(sens);
    store->setStepper(stepper);
    return true;
}

bool LoadFrame::deleteData()
{
    if (store != nullptr) {
        store->deleteLater();
        store = nullptr;
    }
    return true;
}

RETCODE LoadFrame::setTarget(QJsonObject &jOperation)
{
    RETCODE ret = ERROR;
    targetNewtones = Measurements::Force::fromNewtons(jOperation["target"].toDouble());
    // targetMinNewtones = Measurements::Force::fromNewtons(jOperation["target_min"].toDouble());
    ret = controller->setTarget(jOperation, targetNewtones.newtons());      // 200.19642105368277
    return ret;
}

RETCODE LoadFrame::setKPID(QJsonObject &jOperation, AbstractUnit::CMD cmd)
{
    RETCODE ret = ERROR;
    float value = jOperation["value"].toDouble();
    ret = controller->setKPID(jOperation, value, cmd);
    return ret;
}

RETCODE LoadFrame::setHz(QJsonObject &jOperation)
{
    RETCODE ret = ERROR;
    double hz = jOperation["hz"].toDouble();
    ret = controller->setHz(jOperation, hz);
    return ret;
}

// RETCODE LoadFrame::setStatePid(QJsonObject &jOperation)
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

RETCODE LoadFrame::statusSensors(QJsonObject &jOperation)
{
    RETCODE ret = ERROR;
    switch (readSensState) {
    case READ_SENS_1:
        ret = forceSens->read(jOperation);
        if (ret == COMPLATE)
            readSensState = READ_SENS_2;
        break;
    case READ_SENS_2:
        ret = deformSens->read(jOperation);
        if (ret == COMPLATE)
            readSensState = READ_SENS_4; // TODO changed READ_SENS_3
        break;
    case READ_SENS_3:
        ret = deformSens->readRaw(jOperation);
        if (ret == COMPLATE)
            readSensState = READ_SENS_4;
        break;
    case READ_SENS_4:
        ret = stepper->readStatus(jOperation);
        if (ret == COMPLATE)
            readSensState = READ_SENS_5;
        break;
    case READ_SENS_5:
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

void LoadFrame::resetStateModeBusCommunication()
{
    forceSens->resetState();
    deformSens->resetState();
    controller->resetState();
    stepper->resetState();
}

RETCODE LoadFrame::readSensorForce(QJsonObject &jOperation)
{
    return forceSens->read(jOperation);
}

RETCODE LoadFrame::readRawSensorForce(QJsonObject &jOperation)
{
    return forceSens->readRaw(jOperation);
}

RETCODE LoadFrame::readSensorDeform(QJsonObject &jOperation)
{
    return deformSens->read(jOperation);
}

RETCODE LoadFrame::readRawSensorDeform(QJsonObject &jOperation)
{
    return deformSens->readRaw(jOperation);
}

RETCODE LoadFrame::readHolStatus(QJsonObject &jOperation)
{
    return stepper->readStatus(jOperation);
}

RETCODE LoadFrame::readPosition(QJsonObject &jOperation)
{
   return stepper->readPos(jOperation);
}

RETCODE LoadFrame::readControllerStatus(QJsonObject &jOperation)
{
    return controller->readStatus(jOperation);
}

RETCODE LoadFrame::moveFrame(QJsonObject &jobj)
{
    int speed = jobj["speed"].toString().toInt();
    return stepper->setSpeed(jobj, speed);
}

RETCODE LoadFrame::unlockPID(QJsonObject &jobj)
{
    return controller->stopPID(jobj);
}

RETCODE LoadFrame::stopFrame(QJsonObject &jobj)
{
    return stepper->stop(jobj);
}

RETCODE LoadFrame::sensorSetZero(QJsonObject &jobj)
{
    Sensor *sensor = getSensorFromStr(jobj["sensor_name"].toString());
    return sensor->setNull(jobj);
}

RETCODE LoadFrame::resetSensorOffset(QJsonObject &jobj)
{
    Sensor *sensor = getSensorFromStr(jobj["sensor_name"].toString());
    return sensor->resetOffset(jobj);
}

void LoadFrame::readSensors(QJsonObject &jobj)
{
    QJsonObject jsensors;
    jsensors["force"] = QString::number(forceSens->value);
    jsensors["deform"] = QString::number(deformSens->value);
    jsensors["stepper"] = QString::number(stepper->position);
    jobj["sensors"] = jsensors;
    jobj["status"] = "ok";
}

void LoadFrame::sendProtocol(QJsonObject &jobj)
{
    if (store != nullptr) {
        store->sendProtocol(jobj);
    } else {
        jobj["protocol"] = QString(QByteArray("no experiment has been launched yet").toBase64());
    }
}

void LoadFrame::sendStoreData(QJsonObject &jobj)
{
    if (store != nullptr) {

        // jobj["store_data"] = QString(QByteArray(QJsonDocument(jstoreData).toJson()).toBase64());

        store->sendStoreData(jobj);
    } else {
        jobj["store_data"] = QString(QByteArray("no experiment has been launched yet").toBase64());
    }
}
