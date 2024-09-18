#include "storedata.h"
#include "measurements.h"

using namespace Measurements;

Data::Data(quint8 addr, const QJsonObject &conf, QObject *parent) : QObject(parent)
{
    jconfig = conf;
    address = addr;

    createFileProtocol();
    elapseExperimentTimer.start();
}

Data::~Data()
{
    qDebug() << Q_FUNC_INFO;
    fflush(stderr);
}

void Data::createFileProtocol()
{
    QDir current_path = QDir::currentPath();
    QDir protocol_folder = current_path.filePath("%1").arg(address);
    QString file_name = protocol_folder.filePath(QString("INSTR%1-%2-%3.csv")
                                                 .arg(QString::number(address),
                                                      jconfig["name_speciment"].toString(),
                                                      QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm")
                                                 ));
    qDebug() << "checked" << protocol_folder.absolutePath();
    if (!current_path.mkpath(protocol_folder.absolutePath())) {
        qDebug() << "folders found" << current_path;
    } else {
        qDebug() << "create struct folders for protocols";
    }
    dataFileName.setFileName(file_name);

    while (!printFileHeader()) {
        qDebug() << "Warn! File:" << dataFileName.fileName() << "not open";
        fflush(stderr);
        QThread::msleep(300);
    }
}

void Data::setSensors(const QVector<Sensor *> sensors_)
{
    sensors = sensors_;
}

void Data::setStepper(Stepper *stepper_)
{
    stepper = stepper_;
}

bool Data::printFileHeader()
{

    if (!dataFileName.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "protocol file not created" << dataFileName.fileName();
        return false;
    }
    QTextStream str(&dataFileName);

    for(auto i = 0; i < columnNames.size(); ++i) {

        currentData.insert(columnNames[i], std::numeric_limits<double>::quiet_NaN());

        str << columnNames.at(i);

        if(i < columnNames.size() - 1) {
            str << '\t';
        } else {
            str << '\n';
        }
    }
    dataFileName.close();
    return true;
}

void Data::updateData()
{
    for (int i = 0; i < sensors.size(); i++) {
        switch (sensors[i]->funCode) {
        case SensLoad0:
            currentData.insert("VerticalPressure_kPa", sensors[i]->value);
            data["VerticalPressure_kPa"].append(stepTimeStart, elapseExperimentTimer.elapsed(), sensors[i]->value);
            break;

        case SensDef0:
            currentData.insert("VerticalDeform_mm", sensors[i]->value);
            data["VerticalDeform_mm"].append(stepTimeStart, elapseExperimentTimer.elapsed(), sensors[i]->value / 1000.);
            break;

            // Все перепутано это волюмоментр 1
        case SensPrs1:
            currentData.insert("CellPressure_kPa", sensors[i]->value / 1000.);
            data["CellPressure_kPa"].append(stepTimeStart, elapseExperimentTimer.elapsed(), sensors[i]->value / 1000.);
            break;

            // Все перепутано это волюмоментр 2
        case SensPrs0:
            currentData.insert("PorePressure_kPa", sensors[i]->value / 1000.);
            data["PorePressure_kPa"].append(stepTimeStart, elapseExperimentTimer.elapsed(), sensors[i]->value / 1000.);
            break;        }
    }

    currentData.insert("LF_position_mm", stepper->position);
    data["LF_position_mm"].append(stepTimeStart, elapseExperimentTimer.elapsed(), stepper->position);

    if (period.complate())
        writeToDataFile();
}

void Data::sendProtocol(QJsonObject &jobj)
{
    while (!dataFileName.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "protocol not read" << dataFileName.fileName();
        QThread::msleep(300);
    }
    jobj["protocol"] = QString(dataFileName.readAll().toBase64());
    dataFileName.close();
}

void Data::sendStoreData(QJsonObject &jobj)
{
    QJsonObject jstoreData = jobj["store_data"].toObject();
    auto it = data.begin();
    for (; it != data.end(); it++) {
        QJsonObject jData;
        if (jstoreData.contains(it.key())) {
            jData = jstoreData[it.key()].toObject();
            it.value().serializeData(jData);
        } else {
            jData["start_time"] = "-1";
            jData["cur_time"] = "-1";
            it.value().serializeData(jData);
        }
        jstoreData[it.key()] = jData;
    }

    jobj["store_data"] = QString(QByteArray(QJsonDocument(jstoreData).toJson()).toBase64());
}

void Data::setCurStep(const QJsonObject &jcurStep_)
{
    stepTimeStart = elapseExperimentTimer.elapsed();
    jcurStep = jcurStep_;
    period.reset();
}

double Data::getValueStepOfTime(qint64 time, QString sens)
{
    return data[sens].valueFromBack(stepTimeStart, time);
}

bool Data::writeToDataFile()
{
    while (!dataFileName.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        qDebug() << "protocol file not created" << dataFileName.fileName();
        QThread::msleep(300);
    }
    QTextStream str(&dataFileName);

    str.setRealNumberPrecision(3);

    str << elapseExperimentTimer.elapsed() << '\t' <<
           0 << '\t' <<
           QString((jcurStep["step"].toString().isEmpty())?"IDLE":jcurStep["step"].toString()) << '\t' <<
           QString::number(Pressure::fromForce(Force::fromNewtons(currentData.value("VerticalPressure_kPa")), Area::fromMetresSquare(jconfig["area"].toString().toDouble())).kiloPascals()) << '\t' <<
           QString::number(Pressure::fromForce(Force::fromNewtons(currentData.value("ShearPressure_kPa")), Area::fromMetresSquare(jconfig["area"].toString().toDouble())).kiloPascals()) << '\t' <<
           QString::number(currentData.value("CellPressure_kPa")) << '\t' <<
           QString::number(Pressure::fromForce(Force::fromNewtons(currentData.value("PorePressure_kPa")), Area::fromMetresSquare(jconfig["area"].toString().toDouble())).kiloPascals()) << '\t' <<
           QString::number(Pressure::fromForce(Force::fromNewtons(currentData.value("PorePressureAux_kPa")), Area::fromMetresSquare(jconfig["area"].toString().toDouble())).kiloPascals()) << '\t' <<
           QString::number(Length::fromMicrometres(currentData.value("VerticalDeform_mm")).millimetres()) << '\t' <<
           QString::number(Length::fromMicrometres(currentData.value("ShearDeform_mm")).millimetres()) << '\t' <<
           QString::number(currentData.value("CellVolume_mm3")) << '\t' <<
           QString::number(currentData.value("PoreVolume_mm3")) << '\t' <<
           QString::number(currentData.value("Epsilon3")) << "\n";   //Qt:endl for modern kits

    auto result = str.status() == QTextStream::Ok;
    dataFileName.close();
    return result;
}
