#include "experimentview.h"
#include "ui_experimentview.h"
#include "math.h"

ExperimentView::ExperimentView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExperimentView)
{
    ui->setupUi(this);
    ui->dateTimeEdit->setDateTime(QDateTime::fromString("00:00:00"));
    QFont font = ui->lblSensors->font();
    font.setPointSize(42);
    font.setBold(true);
    ui->lblSensors->setFont(font);

    QList<int> size;
    size << 100 << 0;
    ui->splitter->setSizes(size);
    setupPlots();

    connect(&timerIntervalUpdate, &QTimer::timeout, this, &ExperimentView::onReadDataStore);
}

ExperimentView::~ExperimentView()
{
    delete ui;
}

void ExperimentView::setupPlots()
{



    /* First Experiment Plots */
//    auto deformVsTime = new DecoratedPlot(this);
//    QDateTime time = QDateTime::currentDateTime();
//    data1->append(3, time.addMSecs(0));
    dataDeform = new ExperimentData(this);
    deformVsTime = new DecoratedPlot(this);
    deformVsTime->addTrace(dataDeform, tr("Время (t, мин)"), tr("Деформация, %1").arg(Strings::mm));
    ui->tabGraphics->addTab(deformVsTime, tr("Вертикальная деформация (t)"));

    dataPressure = new ExperimentData(this);
    pressureVsTime = new DecoratedPlot(this);
    pressureVsTime->addTrace(dataPressure, tr("Время (t, мин)"), tr("Сила %1").arg(Strings::N));
    ui->tabGraphics->addTab(pressureVsTime, tr("Сила (H)"));

    dataPosition = new ExperimentData(this);
    positionVsTime = new DecoratedPlot(this);
    positionVsTime->addTrace(dataPosition, tr("Время (t, мин)"), tr("Позиция, %1").arg(Strings::mm));
    ui->tabGraphics->addTab(positionVsTime, tr("Позиция"));



}

void ExperimentView::clearData()
{
    dataDeform->clearData();
    dataStore.clear();
    deformVsTime->m_plot->replot();
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
    jRequest["exp_type"] = "compression";
    jRequest["pressure"] = QString::number(ui->spinPressure->value());
    if (ui->comboBox->currentText() == "time") {
        jRequest["criterion"] = "time";
        jRequest.remove("criterion_arg2");
        jRequest.remove("criterion_arg3");
        jRequest["criterion_arg1"] = QString::number(timeInterval(ui->dateTimeEdit->text(), "hh:mm:ss"));
    } else if (ui->comboBox->currentText() == "manual") {
        jRequest["criterion"] = "manual";
        jRequest.remove("criterion_arg1");
        jRequest.remove("criterion_arg2");
        jRequest.remove("criterion_arg3");
    } else if (ui->comboBox->currentText() == "stabilization") {
        jRequest["criterion"] = "stabilization";
        jRequest["criterion_arg3"] = QString::number(ui->spinCriter->value());
        jRequest["criterion_arg2"] = "VerticalDeform_mm";
        jRequest["criterion_arg1"] = QString::number(timeInterval(ui->dateTimeEdit->text(), "hh:mm:ss"));
    }
    ui->textEdit->clear();
    ui->textEdit->append(QJsonDocument(jRequest).toJson());

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

void ExperimentView::onReadSensors()
{
    QJsonObject jobj;
    jobj["CMD"] = "read_sensors";
}

void ExperimentView::on_comboBox_activated(const QString &arg1)
{
    onCreateJsonObject();
}

void ExperimentView::on_dateTimeEdit_timeChanged(const QTime &time)
{
    onCreateJsonObject();
}

void ExperimentView::on_btnAddStep_clicked()
{
    if (ui->textEdit->toPlainText().isEmpty())
        onCreateJsonObject();
    QJsonObject jobj = QJsonDocument::fromJson(ui->textEdit->toPlainText().toUtf8()).object();
    jobj["step"] = QString::number(steps.size());
    QLineEdit *le = new QLineEdit(QJsonDocument(jobj).toJson(), this);
    steps.append(le);

    ui->layoutSteps->addWidget(le);
}

void ExperimentView::on_pushButton_2_clicked()
{
    QJsonObject jconfig;
    jconfig["area"] = QString::number(M_PI * pow(ui->spinBoxDiameter->value() / 2., 2) / 100 / 10000);	// 100мм2=1см2, 1см2=10000м2
    jconfig["name_speciment"] = ui->leNameSpecimen->text();
    QJsonArray jarr;
    for (int i = 0; i < steps.size(); i++) {
        jarr.append(QJsonDocument::fromJson(steps[i]->text().toUtf8()).object());
    }
    jconfig["steps"] = jarr;
    QJsonObject jobj;
    jobj["CMD"] = "settings";
    jobj["config"] = jconfig;

    ui->textEdit->clear();
    ui->textEdit->append(QJsonDocument(jobj).toJson());
    emit sendRequest(jobj);

}

void ExperimentView::on_btnStart_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "start";
    clearData();
    emit sendRequest(jobj);
}

void ExperimentView::onReadyResponse(const QJsonObject &jobj)
{
//    qDebug() << Q_FUNC_INFO << jobj;
//    ui->textEditLogOut->append(QString(QJsonDocument(jobj).toJson()).replace('\n', ' '));
    QString CMD = jobj["CMD"].toString();
    if (CMD == "get_sensor_value") {
        double value = jobj["value"].toString().toDouble();
        dataDeform->append(value);
    }
    if (CMD == "read_sensors") {
        QJsonObject jsensors = jobj["sensors"].toObject();
        QString out_to_lbl;
        QStringList keys = jsensors.keys();
        for (auto &key: keys) {
            if (key == "deform") {
                float deform = jsensors[key].toString().toDouble() / 1000.;
                out_to_lbl.append(QString("%1:\t\t%2\n").arg(key).arg(deform));
//                data1->append(deform, QDateTime::currentDateTime());
            } else if (key == "force") {
                out_to_lbl.append(QString("%1:\t\t%2\n").arg(key).arg(jsensors[key].toString()));
            }
        }
        ui->lblSensors->setText(out_to_lbl);
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
                dataDeform->append(allList);
            }
            if (jkey == "VerticalPressure_kPa") {
                dataPressure->append(allList);
            }
            if (jkey == "Position_mm") {
                dataPosition->append(allList);
            }
        }
        QJsonObject jsensors = jobj["sensors"].toObject();
        QString out_to_lbl;
        QStringList keys = jsensors.keys();
        for (auto &key: keys) {
            if (key == "deform") {
                float deform = jsensors[key].toString().toDouble() / 1000.;
                out_to_lbl.append(QString("%1:\t\t%2\n").arg(key).arg(deform));
//                data1->append(deform, QDateTime::currentDateTime());
            } else if (key == "force") {
                out_to_lbl.append(QString("%1:\t\t%2\n").arg(key).arg(jsensors[key].toString()));
            }
        }
        ui->lblSensors->setText(out_to_lbl);
//        qDebug() << "stop";
//        qDebug() << dataStore;
    }
}

void ExperimentView::on_btnClearLogOut_clicked()
{
    ui->textEditLogOut->clear();
}

void ExperimentView::on_btnClearTextEdit_clicked()
{
    ui->textEdit->clear();
}

void ExperimentView::on_btnMoveUp_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "move_frame";
    jobj["speed"] = "-100";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnMoveDown_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "move_frame";
    jobj["speed"] = "100";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnStopStepper_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "stop_frame";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnUnlockPid_clicked()
{
    QJsonObject jobj;
    jobj["type"] = "client";
    jobj["CMD"] = "unlock_PID";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnSaveImage_clicked()
{
    deformVsTime->m_plot->replot();
    QPixmap pxmap = deformVsTime->grab();
    pxmap.save("test.png", "PNG");
}

void ExperimentView::on_btnGetStoreData_clicked()
{
    onReadDataStore();
}

void ExperimentView::on_btnGetSensorValue_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "get_sensor_value";
    jobj["arg1"] = ui->cmbSensorName->currentText();
    emit sendRequest(jobj);
}


void ExperimentView::on_btnInitStoreData_clicked()
{
    on_btnClearDataStore_clicked();
    QJsonObject jobj;
    jobj["CMD"] = "init_store_data";
    timerIntervalUpdate.start(3000);
    emit sendRequest(jobj);
}


void ExperimentView::on_btnSetSettings_clicked()
{
    QJsonObject jconfig;
    jconfig["area"] = QString::number(M_PI * pow(ui->spinBoxDiameter->value() / 2., 2) / 100 / 10000);	// 100мм2=1см2, 1см2=10000м2
    jconfig["name_speciment"] = ui->leNameSpecimen->text();

    QJsonObject jobj;
    jobj["CMD"] = "settings";
    jobj["config"] = jconfig;

    ui->textEdit->clear();
    ui->textEdit->append(QJsonDocument(jobj).toJson());
    emit sendRequest(jobj);
}


void ExperimentView::on_btnSetTarget_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "set_target";
    jobj["target"] = ui->spinTarget->value();
    emit sendRequest(jobj);
}


void ExperimentView::on_btnClearDataStore_clicked()
{
    dataStore.clear();

    dataDeform->clearData();
    dataDeform->clear();
    deformVsTime->m_plot->replot();

    dataPressure->clearData();
    dataPressure->clear();
    pressureVsTime->m_plot->replot();

    dataPosition->clearData();
    dataPosition->clear();
    positionVsTime->m_plot->replot();
}


void ExperimentView::on_btnSetHz_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "set_hz";
    jobj["hz"] = ui->spinHz->value();
    emit sendRequest(jobj);
}


void ExperimentView::on_btnStopStoreData_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "stop_store_data";
    timerIntervalUpdate.stop();
    emit sendRequest(jobj);
}


void ExperimentView::on_btnHardReset_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "hard_reset";
    emit sendRequest(jobj);
}

