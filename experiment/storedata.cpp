#include "storedata.h"
#include "measurements.h"

using namespace Measurements;

StoreData::StoreData(quint8 addr, const QJsonObject &conf)
{
    jconfig = conf;
    address = addr;

    createFileProtocol();

    elapseExperimentTimer.start();
    elapseStepTimer.start();
}

StoreData::~StoreData()
{
    qDebug() << Q_FUNC_INFO;
}

void StoreData::createFileProtocol()
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

void StoreData::setSensors(const QVector<Sensor *> sensors_)
{
    sensors = sensors_;
}

bool StoreData::printFileHeader()
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

void StoreData::updateData()
{
    for (int i = 0; i < sensors.size(); i++) {
//        QString keySens;
        switch (sensors[i]->funCode) {
        case SensPrs0:
            qDebug() << "pressure0" << sensors[i]->value;
            break;
        case SensPrs1:
            qDebug() << "pressure1" << sensors[i]->value;
            break;
        case SensPrs2:
            qDebug() << "pressure2" << sensors[i]->value;
            break;
        case SensLoad0:
            if (type == LIGA_KL0S_1T) {
                currentData.insert("VerticalPressure_kPa", sensors[i]->value);
                data["VerticalPressure_kPa"].append(stepTimeStart, elapseStepTimer.elapsed(), sensors[i]->value / 1000.);
            } else if (type == LIGA_KL0S_2Load_1T) {
                currentData.insert("VerticalPressure_kPa", sensors[i]->value);
                data["VerticalPressure_kPa"].append(stepTimeStart, elapseStepTimer.elapsed(), sensors[i]->value / 1000.);
            }
            break;
        case SensLoad1:
            if (type == LIGA_KL0S_1T) {
                currentData.insert("VerticalPressure_kPa", sensors[i]->value);
                data["VerticalPressure_kPa"].append(stepTimeStart, elapseStepTimer.elapsed(), sensors[i]->value / 1000.);
            } else if (type == LIGA_KL0S_2Load_1T) {
                currentData.insert("VerticalPressure_kPa", sensors[i]->value);
                data["VerticalPressure_kPa"].append(stepTimeStart, elapseStepTimer.elapsed(), sensors[i]->value / 1000.);
            }
            break;
        case SensDef0:
            if (type == LIGA_KL0S_1T) {
                currentData.insert("VerticalDeform_mm", sensors[i]->value);
                data["VerticalDeform_mm"].append(stepTimeStart, elapseStepTimer.elapsed(), sensors[i]->value / 1000.);
            } else if (type == LIGA_KL0S_2Load_1T) {
                currentData.insert("VerticalDeform_mm", sensors[i]->value);
                data["VerticalDeform_mm"].append(stepTimeStart, elapseStepTimer.elapsed(), sensors[i]->value / 1000.);
            }
            break;
        case SensDef1:
            if (type == LIGA_KL0S_1T) {
                currentData.insert("VerticalDeform_mm", sensors[i]->value);
                data["VerticalDeform_mm"].append(stepTimeStart, elapseStepTimer.elapsed(), sensors[i]->value / 1000.);
            } else if (type == LIGA_KL0S_2Load_1T) {
                currentData.insert("VerticalDeform_mm", sensors[i]->value);
                data["VerticalDeform_mm"].append(stepTimeStart, elapseStepTimer.elapsed(), sensors[i]->value / 1000.);
            }
            break;
        }
    }
    if (period.complate())
        writeToDataFile();
}

void StoreData::sendProtocol(QJsonObject &jobj)
{
    while (!dataFileName.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "protocol not read" << dataFileName.fileName();
        QThread::msleep(300);
    }
    jobj["protocol"] = QString(dataFileName.readAll().toBase64());
    dataFileName.close();
}

void StoreData::sendStoreData(QJsonObject &jobj)
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

//    for (; it != data.end(); it++) {
//        QJsonObject jData;
//        auto itPairList = it.value().data.begin();  // type sensor {deform, forse}
//        for (; itPairList != it.value().data.end(); itPairList++) {

//            QByteArray buff;
//            QDataStream ds(&buff, QIODevice::ReadWrite);
//            ds.setVersion(QDataStream::Qt_5_11);
//            ds.setByteOrder(QDataStream::BigEndian);
//            ds << itPairList.value();
//            jData[QString::number(itPairList.key())] = QString(buff.toBase64());                   // list pairs
//        }
//        jstoreData[it.key()] = jData;
//    }



    jobj["store_data"] = jstoreData;
}

void StoreData::setCurStep(const QJsonObject &jcurStep_)
{
    stepTimeStart = elapseExperimentTimer.elapsed();
    elapseStepTimer.restart();
//    elapseStepTimer.start();
    jcurStep = jcurStep_;
    period.reset();
}

double StoreData::getValueStepOfTime(qint64 time, QString sens)
{
    return data[sens].valueFromBack(stepTimeStart, time);
}

bool StoreData::writeToDataFile()
{
    while (!dataFileName.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        qDebug() << "protocol file not created" << dataFileName.fileName();
        QThread::msleep(300);
    }
    QTextStream str(&dataFileName);

    str.setRealNumberPrecision(3);

    str << elapseExperimentTimer.elapsed() << '\t' <<
           0 << '\t' <<
           jcurStep["step"].toString() << '\t' <<
           QString::number(Pressure::fromForce(Force::fromNewtons(currentData.value("VerticalPressure_kPa")), Area::fromMetresSquare(jconfig["area"].toString().toDouble())).kiloPascals()) << '\t' <<
           QString::number(Pressure::fromForce(Force::fromNewtons(currentData.value("ShearPressure_kPa")), Area::fromMetresSquare(jconfig["area"].toString().toDouble())).kiloPascals()) << '\t' <<
           QString::number(Pressure::fromForce(Force::fromNewtons(currentData.value("CellPressure_kPa")), Area::fromMetresSquare(jconfig["area"].toString().toDouble())).kiloPascals()) << '\t' <<
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
