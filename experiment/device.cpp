#include "device.h"
#include <QRandomGenerator>

using namespace Measurements;

LoadFrame::LoadFrame() :
    /// первая рама
    forceSens(SensLoad0Addr, SensLoad0),
    deformSens(SensDeform0Addr, SensDef0),
    stepper(ActLoad0Addr, ActLoad0),
    controller(CtrlLoad0Addr, CtrlLoad0)
    /// вторая рама
//    forceSens(SensLoad1Addr, SensLoad1),
//    deformSens(SensDeform1Addr, SensDef1),
//    stepper(ActLoad1Addr, ActLoad1),
//    controller(CtrlLoad1Addr, CtrlLoad1)
{
    targetPresure = Measurements::Pressure::fromKiloPascals(50.);
    area.setSiValue(0.004003928);
    timeElapse.start();

}

LoadFrame::~LoadFrame()
{
    qDebug() << Q_FUNC_INFO;
}

void LoadFrame::init()
{
    if (store != nullptr)
        delete store;

    store = new StoreData(address, jconfig);

    QVector<Sensor *> sens;
    sens.append(&forceSens);
    sens.append(&deformSens);

    store->setSensors(sens);
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

RETCODE LoadFrame::next(StatusOperation &operation)
{
    counter++;
    RETCODE ret = ERROR;
    switch (state) {
    case STATE_IDLE:
        if (statusSensors(operation) == COMPLATE) {
            qDebug() << "force =" << Force::fromNewtons(forceSens.value).newtons() << "(N)"
                     << "\t\tpressure =" << Pressure::fromForce(Force::fromNewtons(forceSens.value), area).kiloPascals() << "(kP)"
                     << "\t\tdeform =" << Length::fromMicrometres(deformSens.value).millimetres() << "(mm)\t\traw =" << deformSens.rawValue
                     << "\t\tstepper pos =" << stepper.position
                     << "\t\tcounter =" << counter;
            fflush(stderr);
//            QThread::msleep(1000);
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
            ret = compression(operation);

        if (ret == COMPLATE) {
            state = STATE_READ_NEXT_STEP;
        } else if (ret == ERROR) {
            qDebug() << "Error experiment";
            state = STATE_IDLE;
            return ERROR;
        }
        break;
    case STATE_COMPLATION:
        ret = controller.stopPID(operation);
        if (ret == COMPLATE) {
            qDebug() << "PID stoped";
            state = STATE_FINISH;
        }
        break;
    case STATE_MOVE_FRAME:
        ret = stepper.setSpeed(operation, cur_speed);
        if(ret == COMPLATE) {
            qDebug() << "stepper stop";
            state = STATE_FINISH;
        }
        break;
    case STATE_STOP_FRAME:
        ret = stepper.stop(operation);
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
    return NOERROR;
}

RETCODE LoadFrame::setPresure(StatusOperation &operation)
{
//    double newtones = 0;
    RETCODE ret = ERROR;

    switch (statePresure) {
    case STATE_PRESURE_1:
//        newtones = targetPresure.siValue() * area.siValue();
        targetPresure = Pressure::fromKiloPascals(jcurStep["pressure"].toString().toDouble());
        ret = controller.setTarget(operation, targetPresure.pascals() * area.siValue());      // 200.19642105368277
        if (ret == COMPLATE) {
            statePresure = STATE_PRESURE_2;
        }
        else if (ret == ERROR) {
            statePresure = STATE_PRESURE_1;
            return ERROR;
        }
        break;
    case STATE_PRESURE_2:
        ret = statusSensors(operation);
        if (ret == COMPLATE) {
            double diffPressure = abs(Pressure::fromPascals(forceSens.value / area.siValue()).siValue() - targetPresure.pascals());
            qDebug() << "\n### taretPressure =" << targetPresure.kiloPascals();
            qDebug() << "force =" << Force::fromNewtons(forceSens.value).newtons() << "(N)"
                     << "\t\tpressure =" << Pressure::fromForce(Force::fromNewtons(forceSens.value), area).kiloPascals() << "(kP)"
                     << "\t\tdeform =" << Length::fromMicrometres(deformSens.value).millimetres() << "(mm)"
                     << "\t\tstepper pos =" << stepper.position
                     << "\t\tstepper_status =" << stepper.status
                     << "\t\tcounter =" << counter;
            fflush(stderr);
            if (diffPressure < Measurements::Pressure::fromKiloPascals(1).siValue()) {
                qDebug() << "set" << targetPresure.kiloPascals() << "kP success";
                statePresure = STATE_PRESURE_1;
//                QThread::currentThread()->msleep(15000);
                return COMPLATE;
            }
            QThread::currentThread()->msleep(100);
        }
        else if (ret == ERROR) {
            statePresure = STATE_PRESURE_1;
            return ERROR;
        }
        break;
    }
    return NOERROR;
}

RETCODE LoadFrame::statusSensors(StatusOperation &operation)
{
    switch (readSensState) {
    case READ_SENS_1:
        if (forceSens.read(operation) == COMPLATE)
            readSensState = READ_SENS_2;
        break;
    case READ_SENS_2:
        if (deformSens.read(operation) == COMPLATE)
            readSensState = READ_SENS_3;
        break;
    case READ_SENS_3:
        if (deformSens.readRaw(operation) == COMPLATE)
            readSensState = READ_SENS_4;
        break;
    case READ_SENS_4:
        if (stepper.readStatus(operation) == COMPLATE)
            readSensState = READ_SENS_5;
        break;
    case READ_SENS_5:
        if (stepper.readPos(operation) == COMPLATE)
            readSensState = READ_SENS_6;
        break;
    case READ_SENS_6:
        if (controller.readStatus(operation) == COMPLATE) {
            readSensState = READ_SENS_1;
            if (!jcurStep.empty())
                store->updateData();
            return COMPLATE;
        }
        break;
    }
    return NOERROR;
}

RETCODE LoadFrame::compression(StatusOperation &operation)
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
        ret = statusSensors(operation);
        if (ret == COMPLATE) {
            state_cmprs = STATE_INIT;
            store->updateData();
        }
        break;
    case STATE_INIT:
        ret = setPresure(operation);
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
        ret = criterionTime(operation);
        if (ret == COMPLATE) {
            state_cmprs = STATE_INIT_SENSORS;
            return COMPLATE;
        } else if (ret == ERROR) {
            state_cmprs = STATE_INIT_SENSORS;
            return ERROR;
        }
        break;
    case STATE_CRITERION_STABILIZATION:
        ret = criterionStabilization(operation);
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

RETCODE LoadFrame::criterionTime(StatusOperation &operation)
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
        ret = statusSensors(operation);
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

RETCODE LoadFrame::criterionStabilization(StatusOperation &operation)
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
        ret = statusSensors(operation);
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

RETCODE LoadFrame::criterionManual(StatusOperation &operation)
{
    Q_UNUSED(operation)
    static enum {STATE_INIT} state_criterion_manual = STATE_INIT;
    switch (state_criterion_manual) {
    case STATE_INIT:
        compressionManual = true;
        QThread::msleep(500);
        break;
    }
    return NOERROR;
}

void LoadFrame::readConfig()
{
    QFile file;
    file.setFileName("test.txt");
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

void LoadFrame::startProcess()
{
    state = STATE_START;
}

void LoadFrame::manualNextStep()
{
    state = STATE_READ_NEXT_STEP;
}

void LoadFrame::moveFrame(QJsonObject &jobj)
{
    cur_speed = jobj["speed"].toString().toInt();
    state = STATE_MOVE_FRAME;
}

void LoadFrame::unlockPID()
{
    if (state != STATE_IDLE)
        qDebug() << "WARNING experiment reseting!!!";
    state = STATE_UNLOCK_PID;
}

void LoadFrame::stopFrame()
{
    state = STATE_STOP_FRAME;
}

void LoadFrame::readSensors(QJsonObject &jobj)
{
    QJsonObject jsensors;
    jsensors["force"] = QString::number(forceSens.value);
    jsensors["deform"] = QString::number(deformSens.value);
    jsensors["stepper"] = QString::number(stepper.position);
    if (compressionManual)
        jsensors["status_exp"] = "wait_manual";
    else
        jsensors["status_exp"] = "process";
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
