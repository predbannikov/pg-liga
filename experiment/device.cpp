#include "device.h"
#include <QRandomGenerator>

using namespace Measurements;

#define TIMEWAIT_AFTER_IDLE_STATUS_SENSOR   1000

LoadFrame::LoadFrame() :
    /// первая рама
    forceSens(SensLoad0Addr, SensLoad0),
    deformSens(SensDeform0Addr, SensDef0),
    stepper(ActLoad0Addr, ActLoad0),
    controller(CtrlLoad0Addr, CtrlLoad0)

{
    targetNewtones = Measurements::Force::fromNewtons(50.);
    area.setSiValue(0.004003928);
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

bool LoadFrame::readNextStep()
{
    bool check = false;
    if (!jsteps.isEmpty()) {
        if (jcurStep.isEmpty()) {
            for (const QJsonValue &jval: qAsConst(jsteps))
                if (jval.toObject()["step"].toString().toInt() == 0) {
                    jcurStep = jval.toObject();
                    check = true;
                    break;
                }
        } else {
            for (const QJsonValue &jval: qAsConst(jsteps))
                if (jval.toObject()["step"].toString().toInt() == jcurStep["step"].toString().toInt() + 1) {
                    jcurStep = jval.toObject();
                    check = true;
                    break;
                }
        }
    }
    if (!check) {
        qDebug() << "\n******************************************";
        qDebug() << "* No accessible steps or not load config *";
        qDebug() << "******************************************";
    }
    return check;
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
        state = STATE_READ_NEXT_STEP;
        break;
    case STATE_READ_NEXT_STEP:
        if (readNextStep()) {
            store->setCurStep(jcurStep);
            state = STATE_EXPERIMENT;
        }
        else {
            qDebug() << "Experiment complated\n";
            state = STATE_COMPLATION;
        }
        break;
    case STATE_EXPERIMENT:
        if (jcurStep["exp_type"].toString() == "compression")
            ret = compression(jOperation);

        if (ret == COMPLATE) {
            state = STATE_READ_NEXT_STEP;
        } else if (ret == ERROR) {
            qDebug() << "Error experiment";
            state = STATE_IDLE;
            return ERROR;
        }
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
        jcurStep = QJsonObject();
        jsteps = QJsonArray();
        jconfig = QJsonObject();

        state = STATE_IDLE;
        break;
    }
    return ret;
}

RETCODE LoadFrame::setTarget(QJsonObject &jOperation)
{
//    double newtones = 0;
    RETCODE ret = ERROR;


    targetNewtones = Force::fromNewtons(jOperation["target"].toDouble());
    targetMinNewtones = Force::fromNewtons(jOperation["target_min"].toDouble());
    ret = controller.setTarget(jOperation, targetMinNewtones.newtons(), targetNewtones.newtons());      // 200.19642105368277
//    if (ret == COMPLATE) {
//        statePresure = STATE_PRESURE_2;
//    }
//    else if (ret == ERROR) {
//        statePresure = STATE_PRESURE_1;
////            return ERROR;
//    }


//    switch (statePresure) {
//    case STATE_PRESURE_1:
////        newtones = targetPresure.siValue() * area.siValue();
//        targetNewtones = Force::fromNewtons(jcurStep["target"].toDouble());
//        ret = controller.setTarget(jOperation, targetNewtones.newtons());      // 200.19642105368277
//        if (ret == COMPLATE) {
//            statePresure = STATE_PRESURE_2;
//        }
//        else if (ret == ERROR) {
//            statePresure = STATE_PRESURE_1;
////            return ERROR;
//        }
//        break;
//    case STATE_PRESURE_2:
//        ret = statusSensors(jOperation);
//        if (ret == COMPLATE) {
//            double diffPressure = abs(Pressure::fromPascals(forceSens.value / area.siValue()).siValue() - targetNewtones.pascals());
//            qDebug() << "\n### taretPressure =" << targetNewtones.kiloPascals();
//            qDebug() << "force =" << Force::fromNewtons(forceSens.value).newtons() << "(N)"
//                     << "\t\tpressure =" << Pressure::fromForce(Force::fromNewtons(forceSens.value), area).kiloPascals() << "(kP)"
//                     << "\t\tdeform =" << Length::fromMicrometres(deformSens.value).millimetres() << "(mm)"
//                     << "\t\tstepper pos =" << stepper.position
//                     << "\t\tstepper_status =" << stepper.status
//                     << "\t\tcounter =" << counter;
//            fflush(stderr);
//            if (diffPressure < Measurements::Pressure::fromKiloPascals(1).siValue()) {
//                qDebug() << "set" << targetNewtones.kiloPascals() << "kP success";
//                statePresure = STATE_PRESURE_1;
////                QThread::currentThread()->msleep(15000);
//                return COMPLATE;
//            }
//            QThread::currentThread()->msleep(100);
//        }
//        else if (ret == ERROR) {
//            statePresure = STATE_PRESURE_1;
//            return ERROR;
//        }
//        break;
//    }
//    return NOERROR;
    return ret;
}

RETCODE LoadFrame::setKPID(QJsonObject &jOperation, AbstractUnit::CMD cmd)
{
    RETCODE ret = ERROR;
    float value = jOperation["value"].toDouble();
    ret = controller.setKPID(jOperation, value, cmd);
    return ret;
}

//RETCODE LoadFrame::setPID_D(QJsonObject &jOperation)
//{
//    RETCODE ret = ERROR;
//    float pid_d = jOperation["pid_d"].toDouble();
//    ret = controller.setKd(jOperation, pid_d);
//    return ret;
//}

//RETCODE LoadFrame::setUpPID_P(QJsonObject &jOperation)
//{
//    RETCODE ret = ERROR;
//    float pid_p = jOperation["pid_p"].toDouble();
//    ret = controller.setKPID(jOperation, pid_p);
//    return ret;
//}

RETCODE LoadFrame::setHz(QJsonObject &jOperation)
{
    RETCODE ret = ERROR;
    double hz = jOperation["hz"].toDouble();
    ret = controller.setHz(jOperation, hz);
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

RETCODE LoadFrame::compression(QJsonObject &jOperation)
{
    static enum {STATE_INIT_SENSORS, STATE_INIT, STATE_CRITERION_TIME, STATE_CRITERION_STABILIZATION, STATE_CRITERION_MANUAL} state_cmprs = STATE_INIT_SENSORS;
    RETCODE ret = ERROR;

    if (compressionManual) {
        compressionManual = false;
        state_cmprs = STATE_INIT_SENSORS;
        return COMPLATE;
    }

    switch (state_cmprs) {
    case STATE_INIT_SENSORS:
        ret = statusSensors(jOperation);
        if (ret == COMPLATE) {
            state_cmprs = STATE_INIT;
            store->updateData();
        }
        break;
    case STATE_INIT:
        ret = setTarget(jOperation);
        if (ret == COMPLATE) {
            if (jcurStep["criterion"].toString() == "time") {
                state_cmprs = STATE_CRITERION_TIME;
            }
            else if (jcurStep["criterion"].toString() == "stabilization") {
                state_cmprs = STATE_CRITERION_STABILIZATION;
            } else if (jcurStep["criterion"].toString() == "manual") {
                state_cmprs = STATE_CRITERION_MANUAL;
            }
        } else if (ret == ERROR) {
            state_cmprs = STATE_INIT_SENSORS;
            return ERROR;
        }
        break;
    case STATE_CRITERION_TIME:
        ret = criterionTime(jOperation);
        if (ret == COMPLATE) {
            state_cmprs = STATE_INIT_SENSORS;
            return COMPLATE;
        } else if (ret == ERROR) {
            state_cmprs = STATE_INIT_SENSORS;
            return ERROR;
        }
        break;
    case STATE_CRITERION_STABILIZATION:
        ret = criterionStabilization(jOperation);
        if (ret == COMPLATE) {
            state_cmprs = STATE_INIT_SENSORS;
            return COMPLATE;
        } else if (ret == ERROR) {
            state_cmprs = STATE_INIT_SENSORS;
            return ERROR;
        }
        break;
    case STATE_CRITERION_MANUAL:
        qDebug() << "manual stil not impl";
        break;
    }
    return NOERROR;
}

RETCODE LoadFrame::criterionTime(QJsonObject &jOperation)
{
    static enum {STATE_INIT, STATE_2} state_criterion_time = STATE_INIT;
    RETCODE ret = ERROR;
    switch (state_criterion_time) {
    case STATE_INIT:
        qDebug() << "start criterion time:";
        timeElapse.restart();
        criterionElapseTime.start();
        state_criterion_time = STATE_2;
        break;
    case STATE_2:
        ret = statusSensors(jOperation);
        if (ret == COMPLATE) {
            if (timeElapse.elapsed() > 10000) {
                qDebug() << criterionElapseTime.elapsed() << "ms" << "  criterion=" << jcurStep["criterion_arg1"].toString().toLongLong() * 1000 << " curStep =" << jcurStep["step"].toString();
                timeElapse.restart();
            }
            QThread::msleep(50);
        }
        // Если вышло время независимо от того есть возможность прочитать значения датчиков, переходим на следующий шаг (так как давление уже поднято)
        if (criterionElapseTime.elapsed() > jcurStep["criterion_arg1"].toString().toLongLong() * 1000) {
            qDebug() << "succ";
            state_criterion_time = STATE_INIT;
            return COMPLATE;
        }
        break;
    }
    return NOERROR;
}

RETCODE LoadFrame::criterionStabilization(QJsonObject &jOperation)
{
    static enum {STATE_INIT, STATE_2, STATE_CRITERION_STABILIZATION} state_criterion_stabilization = STATE_INIT;

//    static qint64 timeoutCriterion = 0;

//    static qint64 deltatime =

    RETCODE ret = ERROR;
    switch (state_criterion_stabilization) {
    case STATE_INIT:
        qDebug() << "start criterion stabilization:";
        criterionElapseTime.start();
        timeElapse.restart();
        state_criterion_stabilization = STATE_2;
        break;
    case STATE_2:
        ret = statusSensors(jOperation);
        if (ret == COMPLATE) {
//            if (timeElapse.elapsed() > 1000) {
//                qDebug() << criterionElapseTime.elapsed() << "ms" << "  criterion=" << jcurStep["criterion_arg1"].toString().toLongLong() * 1000;
//                timeElapse.restart();
//            }
            if (timeElapse.elapsed() > 10000) {
                qDebug() << criterionElapseTime.elapsed() <<"ms" << "  criterion=" << jcurStep["criterion_arg1"].toString().toLongLong() * 1000
                         << "\t\tdiff_criter =" << jcurStep["criterion_arg3"].toString().toDouble()
                         << "\t\tcur_difform =" << Length::fromMicrometres(deformSens.value).millimetres()
                         << " curStep =" << jcurStep["step"].toString();

                timeElapse.restart();
            }

            if (criterionElapseTime.elapsed() > jcurStep["criterion_arg1"].toString().toLongLong() * 1000) {
                double lastPoint = store->getValueStepOfTime(jcurStep["criterion_arg1"].toString().toLongLong() * 1000, jcurStep["criterion_arg2"].toString());
                qDebug() << "\nlastPoint =" << lastPoint
                         << "\t\tdiff_criter =" << jcurStep["criterion_arg3"].toString().toDouble()
                         << "\t\tcur_difform =" << Length::fromMicrometres(deformSens.value).millimetres()
                         << "\t\tbuff_size =" << store->data[jcurStep["criterion_arg2"].toString()].size()
                         << "\t\tcounter =" << counter;
                if (abs(lastPoint - Length::fromMicrometres(deformSens.value).millimetres()) < jcurStep["criterion_arg3"].toString().toDouble()) {
                    state_criterion_stabilization = STATE_INIT;
                    qDebug() << "### stabilization complate ###";

                    return COMPLATE;
                }
                qDebug() << "stabilization not performed, wait next interval";
                criterionElapseTime.restart();
                fflush(stderr);
            }
        }
        break;
    case STATE_CRITERION_STABILIZATION:
        break;

    }
    return NOERROR;
}

RETCODE LoadFrame::criterionManual(QJsonObject &jOperation)
{
    Q_UNUSED(jOperation)
    static enum {STATE_INIT} state_criterion_manual = STATE_INIT;
    switch (state_criterion_manual) {
    case STATE_INIT:
        compressionManual = true;
        QThread::msleep(500);
        break;
    }
    return NOERROR;
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
    jsteps = jconfig["steps"].toArray();
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

void LoadFrame::startProcess()
{
    state = STATE_START;
}

void LoadFrame::manualNextStep()
{
    state = STATE_READ_NEXT_STEP;
}

RETCODE LoadFrame::moveFrame(QJsonObject &jobj)
{
    cur_speed = jobj["speed"].toString().toInt();
    return stepper.setSpeed(jobj, cur_speed);
}

RETCODE LoadFrame::unlockPID(QJsonObject &jobj)
{

//    if (state != STATE_IDLE)
//        qDebug() << "WARNING experiment reseting!!!";
//    state = STATE_UNLOCK_PID;
    return controller.stopPID(jobj);
}

RETCODE LoadFrame::stopFrame(QJsonObject &jobj)
{
    return stepper.stop(jobj);
}

RETCODE LoadFrame::hardReset(QJsonObject &jobj)
{
    return plata.write(jobj, HardReset);
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
