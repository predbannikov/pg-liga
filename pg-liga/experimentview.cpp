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

    QList<int> sizes;
    sizes << 100 << 0;
    ui->splitter->setSizes({90, 10});
    setupPlots();
}

ExperimentView::~ExperimentView()
{
    delete ui;
}

void ExperimentView::setupPlots()
{



    /* First Experiment Plots */
//    auto deformVsTime = new DecoratedPlot(this);

    data1 = new ExperimentData(this);
    QDateTime time = QDateTime::currentDateTime();

    data1->append(3, time.addMSecs(0));


    deformVsPressure = new DecoratedPlot(this);
    deformVsPressure->addTrace(data1, tr("Время (t, мин)"), tr("Деформация, %1").arg(Strings::mm));
    ui->tabGraphics->addTab(deformVsPressure, tr("Вертикальная деформация (t)"));

}

qint64 ExperimentView::timeInterval(const QString& date, const QString& format)
{
    QDateTime sourceDate(QDate(1900, 1, 1), QTime(0, 0, 0));
    QDateTime toDt = QDateTime::fromString(date, format);
    QDateTime interval = QDateTime::fromMSecsSinceEpoch(toDt.toMSecsSinceEpoch() - sourceDate.toMSecsSinceEpoch());
    return interval.toSecsSinceEpoch();
}

void ExperimentView::on_pushButton_clicked()
{
    QJsonObject jobj;
    jobj["type"] = "client";
    emit sendRequest(jobj);
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

void ExperimentView::onReadSensors()
{
    QJsonObject jobj;
    jobj["type"] = "client";
    jobj["CMD"] = "read_sensors";
}

void ExperimentView::on_spinPressure_textChanged(const QString &arg1)
{
    onCreateJsonObject();
}

void ExperimentView::on_comboBox_activated(const QString &arg1)
{
    onCreateJsonObject();
}

void ExperimentView::on_spinCriter_textChanged(const QString &arg1)
{
    onCreateJsonObject();
}

void ExperimentView::on_dateTimeEdit_timeChanged(const QTime &time)
{
    onCreateJsonObject();
}

void ExperimentView::on_btnSendRequest_clicked()
{
//    setupPlots();
    QJsonObject jobj;
    jobj["type"] = "client";
    jobj["CMD"] = "get_store_data";
    emit sendRequest(jobj);
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
    jobj["type"] = "client";
    jobj["CMD"] = "settings";
    jobj["config"] = jconfig;

    ui->textEdit->clear();
    ui->textEdit->append(QJsonDocument(jobj).toJson());
}

void ExperimentView::on_btnSendConfig_clicked()
{
    QJsonObject jobj = QJsonDocument::fromJson(ui->textEdit->toPlainText().toUtf8()).object();
    emit sendRequest(jobj);
}

void ExperimentView::on_btnReadConfig_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "read_config";
    jobj["type"] = "client";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnStart_clicked()
{
    QJsonObject jobj;
    jobj["CMD"] = "start";
    jobj["type"] = "client";
    emit sendRequest(jobj);
}

void ExperimentView::onReadyResponse(const QJsonObject &jobj)
{
//    ui->textEditLogOut->append(QString(QJsonDocument(jobj).toJson()).replace('\n', ' '));
    if (jobj["CMD"].toString() == "read_sensors") {
        QJsonObject jsensors = jobj["sensors"].toObject();
        QString out_to_lbl;
        QStringList keys = jsensors.keys();
        for (auto &key: keys) {
            out_to_lbl.append(QString("%1:\t\t%2\n").arg(key).arg(jsensors[key].toString()));
            if (key == "deform")
                data1->append(jsensors[key].toString().toDouble(), QDateTime::currentDateTime());
        }
        ui->lblSensors->setText(out_to_lbl);
    } else if (jobj["CMD"].toString() == "get_protocol") {
        ui->textEdit->append(QByteArray::fromBase64(jobj["protocol"].toString().toUtf8()));
    } else if (jobj["CMD"].toString() == "get_store_data") {
        QJsonObject jstoreData = jobj["store_data"].toObject();
        QMap<QString, QMap<qint64, QList<QPair<qint64, float>>>> complate_data;
        for (const auto &jkey: jstoreData.keys()) {
            QMap <qint64, QList<QPair<qint64, float>>> data;
            QByteArray buff = QByteArray::fromBase64(jstoreData[jkey].toString().toUtf8());
            qDebug() << buff;
            QDataStream ds(&buff, QIODevice::ReadOnly);
            ds.setVersion(QDataStream::Qt_5_11);
            ds.setByteOrder(QDataStream::BigEndian);
            ds >> data;
            complate_data.insert(jkey, data);
        }

    }
}

void ExperimentView::on_btnClearLogOut_clicked()
{
    ui->textEditLogOut->clear();
}

void ExperimentView::on_btnCloseWindow_clicked()
{
    qDebug() << "not implement";
}

void ExperimentView::on_btnGetProtocol_clicked()
{
    QJsonObject jobj;
    jobj["type"] = "client";
    jobj["CMD"] = "get_protocol";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnClearTextEdit_clicked()
{
    ui->textEdit->clear();
}

void ExperimentView::on_btnMoveUp_clicked()
{
    QJsonObject jobj;
    jobj["type"] = "client";
    jobj["CMD"] = "move_frame";
    jobj["speed"] = "-100";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnMoveDown_clicked()
{
    QJsonObject jobj;
    jobj["type"] = "client";
    jobj["CMD"] = "move_frame";
    jobj["speed"] = "100";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnStopStepper_clicked()
{
    QJsonObject jobj;
    jobj["type"] = "client";
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

void ExperimentView::on_btnCloseInstr_clicked()
{
    QJsonObject jobj;
    jobj["type"] = "manager";
    jobj["CMD"] = "close_instr";
    emit sendRequest(jobj);
}

void ExperimentView::on_btnSaveImage_clicked()
{
    deformVsPressure->m_plot->replot();
    QPixmap pxmap = deformVsPressure->grab();
    pxmap.save("test.png", "PNG");
}

void ExperimentView::on_btnSendTestRequest_clicked()
{
    QJsonObject jobj;
    jobj["type"] = "client";
    jobj["CMD"] = "get_store_data";
    emit sendRequest(jobj);
}
