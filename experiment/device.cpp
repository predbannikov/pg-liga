#include "device.h"
#include <QRandomGenerator>

using namespace Measurements;

LoadFrame::LoadFrame() :
    /// первая рама
    forceSens(SensLoad0Addr, SensLoad0),
    deformSens(SensDeform0Addr, SensDef0),
    stepper(ActLoad0Addr, ActLoad0),
    controller(CtrlLoad0Addr, CtrlLoad0)

{
    targetNewtones = Measurements::Force::fromNewtons(50.);
    timeElapse.start();

}

LoadFrame::~LoadFrame()
{
    qDebug() << Q_FUNC_INFO;
}

bool LoadFrame::init()
{
    qDebug() << Q_FUNC_INFO;
    if (store != nullptr)
        delete store;

    store = new StoreData(address, jconfig);

    if (store == nullptr)
        return false;

    QVector<Sensor *> sens;
    sens.append(&forceSens);
    sens.append(&deformSens);


    store->setSensors(sens);
    store->setStepper(&stepper);
    return true;
}

bool LoadFrame::deleteData()
{
    if (store != nullptr) {
        delete store;
        store = nullptr;
    }
}

RETCODE LoadFrame::next(QJsonObject &jOperation)
{
    RETCODE ret = ERROR;
    switch (state) {
    case STATE_IDLE:
        ret = statusSensors(jOperation);
        if (ret == COMPLATE) {
            counter++;
            if (counter % 10 == 0) {
                qDebug() << qPrintable(QString("force=%1(N)\t deform=%2(mm)\t stepperPos=%3\t stepperStatus=%4\t controllerStatus=%5\t counter=%6").
                        arg(Force::fromNewtons(forceSens.value).newtons(), 9).
                        arg(Length::fromMicrometres(deformSens.value).millimetres(), 9).
                        arg((stepper.position * 0.31250)/1000., 9).        // TODO 1:10 на эмуляторе
                        arg(stepper.status).
                        arg(controller.status).
                        arg(counter));
            }
            fflush(stderr);
        }
        break;
    case STATE_START:
        init();
        state = STATE_INIT;
        break;
    case STATE_INIT:
        break;
    case STATE_COMPLATION:
        ret = controller.stopPID(jOperation);
        if (ret == COMPLATE) {
            qDebug() << "PID stoped";
            state = STATE_FINISH;
            store->fixUpdateData();
        }
        break;
    case STATE_MOVE_FRAME:
        ret = stepper.setSpeed(jOperation, cur_speed);
        if(ret == COMPLATE) {
            qDebug() << "stepper stop";
            state = STATE_FINISH;
        }
        break;
    case STATE_STOP_FRAME:
        ret = stepper.stop(jOperation);
        if(ret == COMPLATE) {
            qDebug() << "stepper stop";
            state = STATE_FINISH;
        }
        break;
    case STATE_UNLOCK_PID:
        state = STATE_COMPLATION;
        break;
    case STATE_FINISH:
//        delete store;
//        store = nullptr;
        qDebug() << "\n#STATE_FINISH";
        cur_speed = 0;
        state = STATE_IDLE;
        jconfig = QJsonObject();

        state = STATE_IDLE;
        break;
    }
    return ret;
}

RETCODE LoadFrame::setTarget(QJsonObject &jOperation)
{
    RETCODE ret = ERROR;
    targetNewtones = Force::fromNewtons(jOperation["target"].toDouble());
    targetMinNewtones = Force::fromNewtons(jOperation["target_min"].toDouble());
    ret = controller.setTarget(jOperation, targetMinNewtones.newtons(), targetNewtones.newtons());      // 200.19642105368277
    return ret;
}

RETCODE LoadFrame::setKPID(QJsonObject &jOperation, AbstractUnit::CMD cmd)
{
    RETCODE ret = ERROR;
    float value = jOperation["value"].toDouble();
    ret = controller.setKPID(jOperation, value, cmd);
    return ret;
}

RETCODE LoadFrame::setHz(QJsonObject &jOperation)
{
    RETCODE ret = ERROR;
    double hz = jOperation["hz"].toDouble();
    ret = controller.setHz(jOperation, hz);
    return ret;
}

RETCODE LoadFrame::setStatePid(QJsonObject &jOperation)
{
    RETCODE ret = ERROR;
    STATE_SET_STATE_PID_CONTROLLER state_pid = STATE_SET_STATE_PID_CONTROLLER_DEFAULT;
    if (jOperation["state_pid"].toString() == "P_correct") {
        state_pid = STATE_P_CORRECTION;
    } else if (jOperation["state_pid"].toString() == "controller_default") {
        state_pid = STATE_SET_STATE_PID_CONTROLLER_DEFAULT;
    }
    ret = controller.setStatePidController(jOperation, state_pid);
    return ret;
}

RETCODE LoadFrame::statusSensors(QJsonObject &jOperation)
{
    RETCODE ret = ERROR;
    switch (readSensState) {
    case READ_SENS_1:
        ret = forceSens.read(jOperation);
        if (ret == COMPLATE)
            readSensState = READ_SENS_2;
        break;
    case READ_SENS_2:
        ret = deformSens.read(jOperation);
        if (ret == COMPLATE)
            readSensState = READ_SENS_4; // TODO changed READ_SENS_3
        break;
    case READ_SENS_3:
        ret = deformSens.readRaw(jOperation);
        if (ret == COMPLATE)
            readSensState = READ_SENS_4;
        break;
    case READ_SENS_4:
        ret = stepper.readStatus(jOperation);
        if (ret == COMPLATE)
            readSensState = READ_SENS_5;
        break;
    case READ_SENS_5:
        ret = stepper.readPos(jOperation);
        if (ret == COMPLATE)
            readSensState = READ_SENS_6;
        break;
    case READ_SENS_6:
//        ret = controller.readStatus(jOperation);
        ret = COMPLATE;
        if (ret == COMPLATE) {
            readSensState = READ_SENS_1;
            if (store != nullptr)
                store->updateData();
        }
        break;
    }
    return ret;
}

void LoadFrame::resetStateModeBusCommunication()
{
    forceSens.resetState();
    deformSens.resetState();
    controller.resetState();
    stepper.resetState();
}

void LoadFrame::readConfig()
{
    QFile file;
    file.setFileName("config.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "file not open" << Q_FUNC_INFO;
        return;
    }

    QJsonDocument jdoc = QJsonDocument::fromJson(file.readAll());
    file.close();
    qDebug() << jdoc;
    jconfig = jdoc.object();
}

RETCODE LoadFrame::writeConfig(QJsonObject &jobj)
{
    QFile file;
    file.setFileName("config.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "file not open";
        return ERROR;
    }
    qDebug() << "write new config" << QJsonDocument(jobj["config"].toObject());
    file.write(QJsonDocument(jobj["config"].toObject()).toJson());
    file.close();
    return COMPLATE;
}

RETCODE LoadFrame::moveFrame(QJsonObject &jobj)
{
    cur_speed = jobj["speed"].toString().toInt();
    return stepper.setSpeed(jobj, cur_speed);
}

RETCODE LoadFrame::unlockPID(QJsonObject &jobj)
{
    return controller.stopPID(jobj);
}

RETCODE LoadFrame::stopFrame(QJsonObject &jobj)
{
    return stepper.stop(jobj);
}

RETCODE LoadFrame::sensorSetZero(QJsonObject &jobj)
{
    Sensor &sensor = getSensorFromStr(jobj["sensor_name"].toString());
    return sensor.setNull(jobj);
}

RETCODE LoadFrame::resetSensorOffset(QJsonObject &jobj)
{
    Sensor &sensor = getSensorFromStr(jobj["sensor_name"].toString());
    return sensor.resetOffset(jobj);
}

void LoadFrame::readSensors(QJsonObject &jobj)
{
    QJsonObject jsensors;
    jsensors["force"] = QString::number(forceSens.value);
    jsensors["deform"] = QString::number(deformSens.value);
    jsensors["stepper"] = QString::number(stepper.position);
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
//#define DEBUG_TEST_SERIALIZE_STORE_DATA
#ifdef DEBUG_TEST_SERIALIZE_STORE_DATA
    static bool test_serialize_store_data = true;
    if (test_serialize_store_data) {
        init();
        test_serialize_store_data = false;
    }
    auto generData = [this]() {
        static int counter = 0;
        float test_float = 0.1;
        DataStore dataStore;
        for (int j = 0; j < 10; j++) {
            QList<QPair<qint64, float> > list;
            int tmp = counter;
            for (int i = 1; i < 10; i++) {
                list.append({counter++, test_float});
                test_float += 0.05;
            }
            dataStore.data.insert(tmp, list);
            test_float += 1.0;
        }
        store->data.insert("deform", dataStore);
        store->data.insert("force", dataStore);
    };
    generData();

#endif


    if (store != nullptr) {
        store->sendStoreData(jobj);
    } else {
        jobj["store_data"] = QString(QByteArray("no experiment has been launched yet").toBase64());
    }
}

void LoadFrame::switchToServiceMode(QJsonObject &jobj)
{

}
