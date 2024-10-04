#include "experimentview.h"
#include "ui_experimentview.h"
#include "math.h"

#include "steppedpressurisemodel.h"
#include "clientwindow.h"

ExperimentView::ExperimentView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExperimentView),
    timerUpdateDataStore(new QTimer(this)),
    timerUpdateStatus(new QTimer(this))
{
    ui->setupUi(this);

    QMetaObject::invokeMethod(this, "initServicePanel", Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "initControlPanel", Qt::QueuedConnection);
    setupPlots();

    connect (ui->controlPanelWgt, &ControlPanelWgt::btnStart, [this]() {
        QJsonObject jobj;
        jobj["CMD"] = "start_experiment";
        jobj["curAction"] = "0";
        emit sendRequest(jobj);
    });
    connect (ui->controlPanelWgt, &ControlPanelWgt::btnStop, [this]() {
        QJsonObject jobj;
        jobj["CMD"] = "stop_experiment";
        emit sendRequest(jobj);
    });
    connect (ui->controlPanelWgt, &ControlPanelWgt::btnContinue, [this]() {
        QJsonObject jobj;
        jobj["CMD"] = "continue_experiment";
        emit sendRequest(jobj);
    });
    connect (ui->controlPanelWgt, &ControlPanelWgt::btnPause, [this]() {
        QJsonObject jobj;
        jobj["CMD"] = "pause_experiment";
        emit sendRequest(jobj);
    });


    connect(timerUpdateDataStore, &QTimer::timeout, this, &ExperimentView::onReadDataStore);

    timerUpdateDataStore->start(3000);

    connect(timerUpdateStatus, &QTimer::timeout, this, &ExperimentView::onUpdateStatus);
    timerUpdateStatus->start(1000);



    addOperationActions();



}

void ExperimentView::initServicePanel()
{
    ui->lblLoadFrameSpeed->setNum(ui->sliderLoadFrameSpeed->value());
    ui->lblVolumetr1Speed->setNum(ui->sliderVolumetr1Speed->value());
    ui->lblVolumetr2Speed->setNum(ui->sliderVolumetr2Speed->value());
}

void ExperimentView::initControlPanel()
{
    ClientWindow *clntWgt = qobject_cast<ClientWindow *> (parent());
    clientSetConnectedState(clntWgt->getStatusConnection());
}

ExperimentView::~ExperimentView()
{
    delete ui;
}

void ExperimentView::clientSetConnectedState(bool state)
{
    ui->controlPanelWgt->setConnectionState(state);
}

void ExperimentView::setupPlots()
{
    m_experimentData.insert("Сила", new ExperimentData(this));
    pressureVsTime = new DecoratedPlot(this);
    pressureVsTime->addTrace(m_experimentData.value("Сила"), tr("Время (t, мин)"), tr("Сила %1").arg(Strings::N));
    ui->tabGraphics->addTab(pressureVsTime, tr("Сила (H)"));

    m_experimentData.insert("Деформация", new ExperimentData(this));
    deformVsTime = new DecoratedPlot(this);
    deformVsTime->addTrace(m_experimentData.value("Деформация"), tr("Время (t, мин)"), tr("Деформация, %1").arg(Strings::mm));
    ui->tabGraphics->addTab(deformVsTime, tr("Вертикальная деформация (t)"));

    m_experimentData.insert("Позиция", new ExperimentData(this));
    positionVsTime = new DecoratedPlot(this);
    positionVsTime->addTrace(m_experimentData.value("Позиция"), tr("Время (t, мин)"), tr("Позиция, %1").arg(Strings::mm));
    ui->tabGraphics->addTab(positionVsTime, tr("Позиция"));

    m_experimentData.insert("Всестороннее давление", new ExperimentData(this));
    cellPressureVsTime = new DecoratedPlot(this);
    cellPressureVsTime->addTrace(m_experimentData.value("Всестороннее давление"), tr("Время (t, мин)"), tr("Всестороннее давление, %1").arg(Strings::mm));
    ui->tabGraphics->addTab(cellPressureVsTime, tr("Всестороннее давление"));

    m_experimentData.insert("Сила/деформация", new ExperimentData(this));
    forceVsDeform = new DecoratedPlot(this);
    forceVsDeform->addTrace(m_experimentData.value("Сила/деформация"), tr("Сила (Н)"), tr("деформация, %1").arg(Strings::mm));
    ui->tabGraphics->addTab(forceVsDeform, tr("Сила/деформация"));

    m_experimentData.insert("Поровое давление", new ExperimentData(this));
    porePressureVsTime = new DecoratedPlot(this);
    porePressureVsTime->addTrace(m_experimentData.value("Поровое давление"), tr("Время (t, мин)"), tr("Поровое давление, %1").arg(Strings::mm));
    ui->tabGraphics->addTab(porePressureVsTime, tr("Поровое давление"));

    customPlot = new CustomGraph(&m_experimentData, this);
    ui->tabGraphics->addTab(customPlot, "Кастомный");
}

void ExperimentView::clearData()
{
    m_experimentData.value("Деформация")->clearData();
    dataStore.clear();
    deformVsTime->m_plot->replot();
}

void ExperimentView::onUpdateStatus()
{
    QJsonObject jobj;
    jobj["CMD"] = "get_status_device";
    emit sendRequest(jobj);
}

void ExperimentView::addOperationActions()
{

    QVBoxLayout *lay = qobject_cast<QVBoxLayout *>(ui->scrollAreaWidgetContents->layout());
    OperationActions *opActions = new OperationActions(lay->count()+1, this);
    lay->insertWidget(-1, opActions);

    connect(opActions, &OperationActions::addOperationActions, this, &ExperimentView::addOperationActions);
    connect(opActions, &OperationActions::moveOperationUpActions, this, &ExperimentView::moveUpOperation);
    connect(opActions, &OperationActions::moveOperationDownActions, this, &ExperimentView::moveDownOperation);
    connect(opActions, &OperationActions::dataChanged, this, &ExperimentView::serializExperiment);
    QTimer::singleShot(10, this, &ExperimentView::updateIndexOperationActions);
    connect(opActions, &OperationActions::deleteOperationActions, this, [=] () {
        sender()->deleteLater();
        int countWgts = lay->count();
        if (countWgts == 1)
            this->addOperationActions();
        else
            QTimer::singleShot(10, this, &ExperimentView::updateIndexOperationActions);
    });
}

void ExperimentView::updateIndexOperationActions()
{
    QVBoxLayout *lay = qobject_cast<QVBoxLayout *>(ui->scrollAreaWidgetContents->layout());
    for (int i = 0; i < lay->count(); i++) {
        OperationActions *operAct = qobject_cast<OperationActions *> (lay->itemAt(i)->widget());
        if (operAct) {
            operAct->setNumberOperation(i);
        }
    }
}

void ExperimentView::moveUpOperation()
{
    QVBoxLayout *lay = qobject_cast<QVBoxLayout *>(ui->scrollAreaWidgetContents->layout());
    if (lay->count() < 2)
        return;
    QList <QWidget *> wgts;
    for (int i = 0; i < lay->count(); i++) {
        QWidget *wgt = lay->itemAt(i)->widget();
        if (wgt) {
            wgts.append(wgt);
        }
    }

    while (QLayoutItem *item = lay->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            lay->removeWidget(widget);
        }
        delete item;
    }

    for (int i = 0; i < wgts.count(); i++) {
        if (sender() == wgts[i] && i - 1 >= 0) {
            wgts.move(i, i - 1);
            break;
        }
    }
    for (auto wgt: qAsConst(wgts)) {
        lay->addWidget(wgt);
    }
    QTimer::singleShot(10, this, &ExperimentView::updateIndexOperationActions);
}

void ExperimentView::moveDownOperation()
{
    QVBoxLayout *lay = qobject_cast<QVBoxLayout *>(ui->scrollAreaWidgetContents->layout());
    if (lay->count() < 2)
        return;
    QList <QWidget *> wgts;
    for (int i = 0; i < lay->count(); i++) {
        QWidget *wgt = lay->itemAt(i)->widget();
        if (wgt) {
            wgts.append(wgt);
        }
    }

    while (QLayoutItem *item = lay->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            lay->removeWidget(widget);
        }
        delete item;
    }

    for (int i = 0; i < wgts.count(); i++) {
        if (sender() == wgts[i] && i + 1 < wgts.count()) {
            wgts.move(i, i + 1);
            break;
        }
    }
    for (auto wgt: qAsConst(wgts)) {
        lay->addWidget(wgt);
    }
    QTimer::singleShot(10, this, &ExperimentView::updateIndexOperationActions);
}

void ExperimentView::serializExperiment()
{
    QVBoxLayout *lay = qobject_cast<QVBoxLayout *>(ui->scrollAreaWidgetContents->layout());
    QJsonObject jOps, jObj;
    for (int i = 0; i < lay->count(); i++) {
        OperationActions *operAct = qobject_cast<OperationActions *> (lay->itemAt(i)->widget());
        QString cnt = QString("operation_%1").arg(i);
        QJsonObject jOperation = operAct->serializOperation();
        jOps[cnt] = jOperation;
    }
    jObj["CMD"] = "set_experiment";
    jObj["experiment"] = jOps;
    qDebug().noquote() << QJsonDocument(jObj).toJson(QJsonDocument::Indented);
    emit sendRequest(jObj);
}

void ExperimentView::updateStatusView(QJsonObject jObj)
{
    auto jSensors = jObj["sensors"].toObject();

    QString out_to_lbl;

    for (QJsonObject::iterator iter = jSensors.begin(); iter != jSensors.end(); ++iter) {
        out_to_lbl.append(QString("%1:%2\n").arg(iter.key(), 15).arg(iter.value().toString() ));
        if (iter.key() == "LF_sensor_force")
            ui->lblLoadFrameForceSensor->setText(iter.value().toString());
        if (iter.key() == "LF_sensor_deform")
            ui->lblLoadFrameDeformSensor->setText(iter.value().toString());
        if (iter.key() == "LF_sensor_hol") {
            if (iter.value().toString().toInt() == 0) {
                ui->btnLoadFrameMoveUp->setStyleSheet("background-color: #66cc99;");
                ui->btnLoadFrameMoveDown->setStyleSheet("background-color: #66cc99;");
            } else if (iter.value().toString().toInt() == 1) {
                ui->btnLoadFrameMoveUp->setStyleSheet("background-color: #66cc99;");
                ui->btnLoadFrameMoveDown->setStyleSheet("background-color: #ffaaaa;");
            } else if (iter.value().toString().toInt() == 2) {
                ui->btnLoadFrameMoveUp->setStyleSheet("background-color: #ffaaaa;");
                ui->btnLoadFrameMoveDown->setStyleSheet("background-color: #66cc99;");
            }
        }
        if (iter.key() == "LF_stepper_pos")
            ui->lblLoadFrameForceSensorPosition->setText(iter.value().toString());
//        if (iter.key() == "LF_controller_status")
//            ui->lblLoadFrameForceSensorPosition->setText(iter.value().toString());


        if (iter.key() == "Vol1_sensor_pressure")
            ui->lblVolumetr1PressureSensor->setText(iter.value().toString());
        if (iter.key() == "Vol1_sensor_hol") {
            if (iter.value().toString().toInt() == 0) {
                ui->btnVolumetr1MoveUp->setStyleSheet("background-color: #66cc99;");
                ui->btnVolumetr1MoveDown->setStyleSheet("background-color: #66cc99;");
            } else if (iter.value().toString().toInt() == 1) {
                ui->btnVolumetr1MoveUp->setStyleSheet("background-color: #ffaaaa;");
                ui->btnVolumetr1MoveDown->setStyleSheet("background-color: #66cc99;");
            } else if (iter.value().toString().toInt() == 2) {
                ui->btnVolumetr1MoveUp->setStyleSheet("background-color: #66cc99;");
                ui->btnVolumetr1MoveDown->setStyleSheet("background-color: #ffaaaa;");
            }
        }
        if (iter.key() == "Vol1_stepper_pos")
            ui->lblVolumetr1SensorPosition->setText(iter.value().toString());
//        if (iter.key() == "Vol1_controller_status")
//            ui->grpManualMode->setEnabled(false);

        if (iter.key() == "Vol2_sensor_pressure")
            ui->lblVolumetr2PressureSensor->setText(iter.value().toString());
        if (iter.key() == "Vol2_sensor_hol") {
            if (iter.value().toString().toInt() == 0) {
                ui->btnVolumetr2MoveUp->setStyleSheet("background-color: #66cc99;");
                ui->btnVolumetr2MoveDown->setStyleSheet("background-color: #66cc99;");
            } else if (iter.value().toString().toInt() == 1) {
                ui->btnVolumetr2MoveUp->setStyleSheet("background-color: #ffaaaa;");
                ui->btnVolumetr2MoveDown->setStyleSheet("background-color: #66cc99;");
            } else if (iter.value().toString().toInt() == 2) {
                ui->btnVolumetr2MoveUp->setStyleSheet("background-color: #66cc99;");
                ui->btnVolumetr2MoveDown->setStyleSheet("background-color: #ffaaaa;");
            }
        }
        if (iter.key() == "Vol2_stepper_pos")
            ui->lblVolumetr2SensorPosition->setText(iter.value().toString());
//        if (iter.key() == "Vol2_controller_status")
//            ui->grpManualMode->setEnabled(false);


        out_to_lbl.append("\n");

    }

    if (out_to_lbl.size() < 5)
        qDebug() << "stop";
    out_to_lbl.append("\n");
    out_to_lbl.append(jObj["status_experiment"].toString());
    ui->lblSensors->setText(out_to_lbl);
    ui->controlPanelWgt->changeButtons(jObj["status_experiment"].toString());
}

qint64 ExperimentView::timeInterval(const QString& date, const QString& format)
{
    QDateTime sourceDate(QDate(1900, 1, 1), QTime(0, 0, 0));
    QDateTime toDt = QDateTime::fromString(date, format);
    QDateTime interval = QDateTime::fromMSecsSinceEpoch(toDt.toMSecsSinceEpoch() - sourceDate.toMSecsSinceEpoch());
    return interval.toSecsSinceEpoch();
}

void ExperimentView::onCreateJsonObject()
{
    QJsonDocument jdoc = QJsonDocument::fromJson(ui->textEdit->toPlainText().toUtf8());
    jRequest = QJsonObject();
}

void ExperimentView::onReadDataStore()
{
    QJsonObject jobj;
    jobj["CMD"] = "get_store_data";
    QJsonObject jDataStore;
    if (!dataStore.isEmpty()) {
        for (const auto &key: dataStore.keys()) {
            QJsonObject jData;
            qint64 start_time = -1;
            qint64 cur_time = -1;
            auto &dStore = dataStore[key];
            dStore.getLastStartAndCurTime(start_time, cur_time);
            jData["start_time"] = QString::number(start_time);
            jData["cur_time"] = QString::number(cur_time);
            jDataStore[key] = jData;
        }
    }
    jobj["store_data"] = jDataStore;
    emit sendRequest(jobj);
}

void ExperimentView::on_btnInitStoreData_clicked()
{
    on_btnClearDataStore_clicked();
    QJsonObject jobj;
    jobj["CMD"] = "init_store_data";
    timerUpdateDataStore->start(3000);
    emit sendRequest(jobj);
}

void ExperimentView::on_btnStopStoreData_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "stop_store_data";
    timerUpdateDataStore->stop();
    emit sendRequest(jobj);
}

void ExperimentView::on_btnEnableStoreData_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "enable_store_data";
    timerUpdateDataStore->start(3000);
    emit sendRequest(jobj);
}


void ExperimentView::on_btnClearDataStore_clicked()
{
    dataStore.clear();

    m_experimentData.value("Деформация")->clearData();
    m_experimentData.value("Деформация")->clear();
    deformVsTime->m_plot->replot();

    m_experimentData.value("Сила")->clearData();
    m_experimentData.value("Сила")->clear();
    pressureVsTime->m_plot->replot();

    m_experimentData.value("Позиция")->clearData();
    m_experimentData.value("Позиция")->clear();
    positionVsTime->m_plot->replot();

    m_experimentData.value("Всестороннее давление")->clearData();
    m_experimentData.value("Всестороннее давление")->clear();
    cellPressureVsTime->m_plot->replot();

    m_experimentData.value("Сила/деформация")->clearData();
    m_experimentData.value("Сила/деформация")->clear();
    forceVsDeform->m_plot->replot();

    m_experimentData.value("Поровое давление")->clearData();
    m_experimentData.value("Поровое давление")->clear();
    porePressureVsTime->m_plot->replot();

    customPlot->clear();

    QJsonObject jobj;
    jobj["CMD"] = "delete_store_data";
    emit sendRequest(jobj);
}

void ExperimentView::on_comboBox_activated(const QString &arg1)
{
    onCreateJsonObject();
}

void ExperimentView::on_dateTimeEdit_timeChanged(const QTime &time)
{
    onCreateJsonObject();
}

void ExperimentView::on_btnGetStoreData_clicked()
{
    onReadDataStore();
}


void ExperimentView::onReadyResponse(const QJsonObject &jobj)
{
    //    qDebug() << Q_FUNC_INFO << jobj;
    //    ui->textEditLogOut->append(QString(QJsonDocument(jobj).toJson()).replace('\n', ' '));
    QString CMD = jobj["CMD"].toString();
    if (CMD == "get_status_device") {
        updateStatusView(jobj);

    } else if (CMD == "get_protocol") {
        ui->textEdit->append(QByteArray::fromBase64(jobj["protocol"].toString().toUtf8()));

    } else if (CMD == "get_store_data") {
        QJsonObject jstoreData = QJsonDocument::fromJson(QByteArray::fromBase64(jobj["store_data"].toString().toUtf8())).object();
        //        qDebug() << Q_FUNC_INFO << "jstoreData" << jstoreData;
        for (const auto &jkey: jstoreData.keys()) {
            if (!dataStore.contains(jkey)) {
                //                qDebug() << QString("key%1    value%2").arg(jkey).arg(QJsonDocument::fromJson(QByteArray::fromBase64(jstoreData.value(jkey).toString().toUtf8())).object());
                dataStore.insert(jkey, DataStore());
            }
            QList<QPair<qint64, float>> allList = dataStore[jkey].deSerializeData(jstoreData[jkey].toObject());
            //            qDebug() << Q_FUNC_INFO << "allList" << jkey << allList;
            if (jkey == "VerticalDeform_mm") {
                m_experimentData.value("Деформация")->appendListOfTime(allList);
            }
            if (jkey == "VerticalPressure_kPa") {
                m_experimentData.value("Сила")->appendListOfTime(allList);
            }
            if (jkey == "LF_position_mm") {
                m_experimentData.value("Позиция")->appendListOfTime(allList);
            }
            if (jkey == "PorePressure_kPa") {
                m_experimentData.value("Поровое давление")->appendListOfTime(allList);
            }
            if (jkey == "CellPressure_kPa") {
                m_experimentData.value("Всестороннее давление")->appendListOfTime(allList);
            }
            if (jkey == "VertPressureVsVertDeform") {
                m_experimentData.value("Сила/деформация")->appendListOfValue(allList);
            }
        }
    } else {
        qDebug().noquote() << QJsonDocument(jobj).toJson(QJsonDocument::Indented);
        ui->textEdit->append(QJsonDocument(jobj).toJson(QJsonDocument::Indented));
        ui->textEdit->append("##################################################");
        ui->textEdit->append("                                                  ");
        ui->textEdit->append("##################################################");
    }
    if (jobj.contains("msg"))
        ui->textEdit->append(jobj["msg"].toString());
}

void ExperimentView::on_btnUnlockPid_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "unlock_PID";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnSaveImage_clicked()
{
    deformVsTime->m_plot->replot();
    QPixmap pxmap = deformVsTime->grab();
    pxmap.save("test.png", "PNG");
}


void ExperimentView::on_btnSetSettings_clicked()
{
    QJsonObject jconfig;
//    jconfig["area"] = QString::number(M_PI * pow(ui->spinBoxDiameter->value() / 2., 2) / 100 / 10000);	// 100мм2=1см2, 1см2=10000м2
//    jconfig["name_speciment"] = ui->leNameSpecimen->text();

    QJsonObject jobj;
//    jobj["CMD"] = "settings";
//    jobj["config"] = jconfig;

    ui->textEdit->clear();
    ui->textEdit->append(QJsonDocument(jobj).toJson());
//    emit sendRequest(jobj);
}

void ExperimentView::on_btnSetHz_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "set_hz";
    jobj["hz"] = ui->spinHz->value();
    emit sendRequest(jobj);
}

void ExperimentView::on_btnHardReset_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "hard_reset";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnSetPidP_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "set_pid_p";
    jobj["value"] = ui->spinSetPID_P->value();
    emit sendRequest(jobj);
}

void ExperimentView::on_btnSetPID_D_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "set_pid_d";
    jobj["value"] = ui->spinSetPID_D->value();
    emit sendRequest(jobj);
}

void ExperimentView::on_btnSetUpPidP_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "set_up_pid_p";
    jobj["value"] = ui->spinSetPID_P->value();
    emit sendRequest(jobj);
}

void ExperimentView::on_btnSetUpPID_D_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "set_up_pid_d";
    jobj["value"] = ui->spinSetPID_D->value();
    emit sendRequest(jobj);
}

void ExperimentView::on_btnSetState_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "set_state_pid";
    jobj["state_pid"] = ui->cmbState->currentText();
    emit sendRequest(jobj);
}

void ExperimentView::on_btnLoadFrameMoveUp_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "load_frame_move";
    jobj["speed"] = QString::number(-ui->sliderLoadFrameSpeed->value());
    emit sendRequest(jobj);
}

void ExperimentView::on_btnLoadFrameMoveDown_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "load_frame_move";
    jobj["speed"] = QString::number(ui->sliderLoadFrameSpeed->value());
    emit sendRequest(jobj);
}

void ExperimentView::on_btnLoadFrameStopStepper_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "load_frame_stop";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnTest_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "set_experiment";
    QJsonObject statOp;
    statOp["status"] = "";
    QJsonObject jExp;
    for (int i = 0; i < 10; i+=2) {
        QJsonObject jAction;
        jAction["name"] = "move_of_time";
//        jAction["speed"] = QString::number(10 * (i+1));
        jAction["speed"] = QString::number(100);
        jAction["time_ms"] = "1500";
        jAction["status"] = "";
        jExp[QString("Operation_%1").arg(i)] = jAction;

        QJsonObject jAction2;
        jAction2["name"] = "move_of_time";
//        jAction2["speed"] = QString::number(-10 * (i+1));
        jAction2["speed"] = QString::number(-100);
        jAction2["time_ms"] = "1500";
        jAction2["status"] = "";
        jExp[QString("Operation_%1").arg(i+1)] = jAction2;
    }
    QJsonObject jStatus;
    jStatus["state"] = "idle";
    jStatus["curAction"] = "0";
    jExp["status"] = jStatus;
    jobj["experiment"] = jExp;
    qDebug().noquote() << QJsonDocument(jExp).toJson(QJsonDocument::Indented);

    emit sendRequest(jobj);
}

void ExperimentView::on_btnLoadFrameSensorForceSetZero_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "load_frame_sensor_set_zero";
    jobj["sensor_name"] = "SensLoad0";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnLoadFrameSensorForceReset_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "load_frame_sensor_reset_offset";
    jobj["sensor_name"] = "SensLoad0";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnLoadFrameSensorPositionSetZero_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "load_frame_stepper_set_zero";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnLoadFrameSensorDeformSetZero_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "load_frame_sensor_set_zero";
    jobj["sensor_name"] = "SensDef0";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnLoadFrameSensorDeformReset_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "load_frame_sensor_reset_offset";
    jobj["sensor_name"] = "SensDef0";
    emit sendRequest(jobj);
}


void ExperimentView::on_btnLoadFrameSetTarget_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "load_frame_set_target";
    jobj["target"] = QString::number(ui->spinLoadFrameTargetPID->value());
    emit sendRequest(jobj);
}


void ExperimentView::on_btnLoadFrameSetVibro_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "load_frame_set_vibro";
    jobj["target"] = QString::number(ui->spinLoadFrameTargetPID->value());
    jobj["target_min"] = QString::number(ui->spinLoadFrameTargetPIDMin->value());
    emit sendRequest(jobj);
}

void ExperimentView::on_btnLoadFrameUnlockPid_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "load_frame_unlock_PID";
    emit sendRequest(jobj);
}


void ExperimentView::on_sliderLoadFrameSpeed_valueChanged(int value)
{
    ui->lblLoadFrameSpeed->setText(QString::number(value));
}



void ExperimentView::on_btnVolumetr1SensorPressureSetZero_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "volumetr1_sensor_set_zero";
    jobj["sensor_name"] = "SensPrs0";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnVolumetr1SensorPressureReset_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "volumetr1_sensor_reset_offset";
    jobj["sensor_name"] = "SensPrs0";
    emit sendRequest(jobj);
}


void ExperimentView::on_btnVolumetr1SensorPositionSetZero_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "volumetr1_stepper_set_zero";
    emit sendRequest(jobj);
}


void ExperimentView::on_btnVolumetr1SetTarget_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "volumetr1_set_target";
    jobj["target"] = QString::number(ui->spinVolumetr1Target->value());
    emit sendRequest(jobj);
}

void ExperimentView::on_btnVolumetr1Unlock_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "volumetr1_unlock_PID";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnVolumetr1MoveUp_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "volumetr1_move";
    jobj["speed"] = QString::number(ui->sliderVolumetr1Speed->value());
    emit sendRequest(jobj);
}

void ExperimentView::on_btnVolumetr1MoveDown_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "volumetr1_move";
    jobj["speed"] = QString::number(-ui->sliderVolumetr1Speed->value());
    emit sendRequest(jobj);
}

void ExperimentView::on_btnVolumetr1Stop_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "volumetr1_stop";
    emit sendRequest(jobj);
}


void ExperimentView::on_btnVolumetr2MoveUp_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "volumetr2_move";
    jobj["speed"] = QString::number(ui->sliderVolumetr2Speed->value());
    emit sendRequest(jobj);
}


void ExperimentView::on_btnVolumetr2Stop_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "volumetr2_stop";
    emit sendRequest(jobj);
}


void ExperimentView::on_btnVolumetr2MoveDown_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "volumetr2_move";
    jobj["speed"] = QString::number(-ui->sliderVolumetr2Speed->value());
    emit sendRequest(jobj);
}


void ExperimentView::on_sliderVolumetr2Speed_valueChanged(int value)
{
    ui->lblVolumetr2Speed->setText(QString::number(value));
}


void ExperimentView::on_sliderVolumetr1Speed_valueChanged(int value)
{
    ui->lblVolumetr1Speed->setText(QString::number(value));
}



void ExperimentView::on_btnVolumetr2SensorPositionSetZero_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "volumetr2_stepper_set_zero";
    emit sendRequest(jobj);
}


void ExperimentView::on_btnVolumetr2SensorPressureSetZero_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "volumetr2_sensor_set_zero";
    jobj["sensor_name"] = "SensPrs1";
    emit sendRequest(jobj);
}


void ExperimentView::on_btnVolumetr2SensorPressureReset_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "volumetr2_sensor_reset_offset";
    jobj["sensor_name"] = "SensPrs1";
    emit sendRequest(jobj);
}


void ExperimentView::on_btnVolumetr2SetTarget_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "volumetr2_set_target";
    jobj["target"] = QString::number(ui->spinVolumetr2Target->value());
    emit sendRequest(jobj);
}


void ExperimentView::on_btnVolumetr2Unlock_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "volumetr2_unlock_PID";
    emit sendRequest(jobj);
}



void ExperimentView::on_btnClearTextEdit_clicked()
{
    ui->textEdit->clear();
}

