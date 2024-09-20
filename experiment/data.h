#ifndef DATA_H
#define DATA_H

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

const QString VerticalPressure_kPa = "VerticalPressure_kPa";
const QString CellPressure_kPa = "CellPressure_kPa";
const QString PorePressure_kPa = "PorePressure_kPa";
const QString VerticalDeform_mm = "VerticalDeform_mm";
const QString LF_position_mm = "LF_position_mm";

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

class Data : public QObject
{
    const QStringList columnNames = {
        "Time",
        "Mode",
        "Step",
        VerticalPressure_kPa,
        "ShearPressure_kPa",
        CellPressure_kPa,
        PorePressure_kPa,
        "PorePressureAux_kPa",
        VerticalDeform_mm,
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
    void createFileProtocol(QString nameOperation);

public:
    Data(quint8 addr, const QJsonObject &conf, QObject *parent = nullptr);
    ~Data();

    void setSensors(const QVector <Sensor*> sensors_);
    void setStepper(Stepper *stepper_);

    bool printFileHeader();

    void updateData();
    void sendProtocol(QJsonObject &jobj);
    void sendStoreData(QJsonObject &jobj);
    void startOperation(QJsonObject &jObj);
    void stopOperation(QJsonObject &jObj);
    void startStep(QJsonObject &jStatusStep);
    void stopStep(QJsonObject &jStatusStep);

    // Вернуть значение по сенсору time времени назад
    QPair<bool, double> getValueStepOfTime(qint64 time, QString sens);
    void startExperiment(QJsonObject &jObj);
    void stopExperiment(QJsonObject &jObj);


    QMap <QString, DataStore*> data;
    QString enableStoreData(bool enable);
    QString deleteStoreData();
    QString initStoreData();
private:
    QFile dataFileName;
    quint8 address;
    QJsonObject jconfig;
    QJsonObject jcurStep;
    QVector <Sensor*> sensors;
    Stepper *stepper;

    /**
     * @brief writeToDataFile       Запись в протокол
     * @return
     */
    bool writeToDataFile();

    /**
     *      Время текущее на протяжении всего эксперимента, испльзовать для задачи ключей
     *      в мапе как step в которых хранятся данные датчиков
     */
    QElapsedTimer elapseExperimentTimer;

    qint64 stepTime = 0;
    TimePeriod period;      // Период обновления значений в протокол
    bool enableStore = false;
    QString timeStartExperiment;
    QString timeStopExperiment;
    QString timeStartOperation;
    QString timeStopOperation;
};

#endif // DATA_H
