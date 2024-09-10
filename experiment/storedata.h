#ifndef STOREDATA_H
#define STOREDATA_H

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "sensors.h"
#include "stepper.h"
#include <limits>
#include <QList>
#include <QElapsedTimer>
#include <QDataStream>
#include <QMap>

#include "datastore.h"

//class Data
//{
//    QPair <qint64, float> last;
//public:
//    Data() {

//    };
//    ~Data() {

//    };

//    float valueFromBack(qint64 step_time, qint64 time) {
//        auto &arr = data[step_time];
////        QPair <qint64, float> last_value = arr.last();
//        for (int i = arr.size() - 1; i > 0; i--) {
////            if (last_value.first - arr[i].first >= time) {
//            if (last.first - arr[i].first >= time) {
//                return arr[i].second;
//            }
//        }
//        qDebug() << Q_FUNC_INFO << "#WARNING: get first value stop";
//        return arr.first().second;
//    }

//    void append(qint64 start_time, qint64 cur_time, float value, float eps = 0.01) {
//        if (!data.contains(start_time)) {
//            data.insert(start_time, {{cur_time, value}});
//        } else {
//            if (abs (data[start_time].last().second - (value)) > eps) {
//                if (last.first != data[start_time].last().first)
//                    data[start_time].append(last);
//                data[start_time].append({cur_time, value});
//            }
//            last = {cur_time, value};
//        }
//    }

//    qint64 size() {
//        qint64 sz = 0;
//        for (auto &lst: data) {
//            sz += lst.size();
//        }
//        return sz;
//    }
//    QMap <qint64, QList<QPair<qint64, float>>> data;
//};

struct TimePeriod {
    const QVector<qint64> periods;
    TimePeriod() :
        periods({
                0,
                1, 1, 1,
                3, 3, 3,
                6, 6, 6,
                15, 15, 15,
                30, 30, 30,
                60, 60, 60,
                120, 120, 120,
                300, 300, 300,
                600, 600, 600,
                1200, 1200, 1200,
                1800
            })
    {
        elapsePeriod.start();
    }
    void reset() {index = 0; elapsePeriod.restart();}
    bool complate() {
        if (elapsePeriod.elapsed() > periods[index] * 1000) {
            if (index < periods.size() - 1)
                index++;
            elapsePeriod.restart();
            return true;
        }
        return false;;
    }
    int index = 0;
private:
    QElapsedTimer elapsePeriod;
};

class StoreData : public QObject
{



    enum InstrumentType{
        LIGA_KL0_1T,
        LIGA_KL1_1Vol_1T,
        LIGA_KL1_2Vol_1T,
        LIGA_KL0S_1T,
        LIGA_KL0S_2Load_1T,
        LIGA_KL1S_2Vol_1T,
        LIGA_KL0_5T,
        LIGA_KL1_1Vol_5T,
        LIGA_KL1_2Vol_5T,
        LIGA_KL0_25T,
        LIGA_KL1_1Vol_25T
    };


    const QStringList columnNames = {
        "Time",
        "Mode",
        "Step",
        "VerticalPressure_kPa",
        "ShearPressure_kPa",
        "CellPressure_kPa",
        "PorePressure_kPa",
        "PorePressureAux_kPa",
        "VerticalDeform_mm",
        "ShearDeform_mm",
        "CellVolume_mm3",
        "PoreVolume_mm3",
        "Epsilon3"
    };

    /**
     * @brief currentData       Данные текущие, постоянно обновляются
     *                          используется для записи в файл протокола
     */
    QMap <QString, double> currentData;
    void createFileProtocol();

public:
    StoreData(quint8 addr, const QJsonObject &conf, QObject *parent = nullptr);
    ~StoreData();

    void setSensors(const QVector <Sensor*> sensors_);
    void setStepper(Stepper *stepper_);

    bool printFileHeader();

    void updateData();
    void fixUpdateData();
    void sendProtocol(QJsonObject &jobj);
    void sendStoreData(QJsonObject &jobj);
    void setCurStep(const QJsonObject &jcurStep_);

//    void clearStore(const QString name);

    double getValueStepOfTime(qint64 time, QString sens);



    QMap <QString, DataStore> data;
private:
    QFile dataFileName;
    quint8 address;
    QJsonObject jconfig;
    QJsonObject jcurStep;
    QVector <Sensor*> sensors;
    Stepper *stepper;

    InstrumentType type = LIGA_KL0_1T;
    bool writeToDataFile();

    QElapsedTimer elapseExperimentTimer;
    qint64 stepTimeStart = 0;
//    QElapsedTimer elapseStepTimer;

    TimePeriod period;
};

#endif // STOREDATA_H
