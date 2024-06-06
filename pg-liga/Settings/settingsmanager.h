#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QMap>
#include <QObject>

#include "measurements/measurements.h"



struct InstrumentParameters {

    enum ChamberType {
        ChamberTypeA,
        Oedometer
    };

    QString instrumentName; //VibroStabilometer;
    int pollPeriodPort = 1000;

    /**
     * @brief The Reducer struct
     *          stepDistance - микрометров на один шаг ШД
     *          maxSpeed - максимальная допустимая скорость ШД
     */
    struct Reducer {
        QString gearName;
        Measurements::Length stepDistance;
        int maxSpeed;
    };

    struct PIDParameters {
        double p = 0.3;
        double i = 0.3;
        double d = 0.3;
    };

    struct UnitChannel {
        UnitChannel(QString load = "", QString deform = "", QString act = "", QString ctrl = "") :
            loading(load),
            deformation(deform),
            actuator(act),
            controller(ctrl)
        {}
        QString loading = "SensLoad0";
        QString deformation = "SensDeform0";
        QString actuator = "ActLoad0";
        QString controller = "CtrlLoad0";
    };

    struct FilterKalman {
        bool enabled = false;
        float range = 3.0;
        float speedReaction = 3.0;
    };

    struct TriaxialChamberParameters {
        int type = ChamberTypeA;
        Measurements::Length pistonDiameter = Measurements::Length::fromMillimetres(16);
        Measurements::Force pistonWeight = Measurements::Force::fromNewtons(20.0);
    };

    struct Unit {
        Reducer reducer;
        Measurements::Length distance = Measurements::Length::fromMillimetres(50);
        UnitChannel unitChannel = {"SensLoad0", "SensDeform0", "ActLoad0", "CtrlLoad0"};
        double scalingPIDAdjastFrame = 15.0;
        double scalingPIDVolumeterExperiment = 100.0;
        FilterKalman filter;
        double multipicationCoefficient = 1.0;
        Measurements::Pressure maxPressure = Measurements::Pressure::fromKiloPascals(2800);
        PIDParameters pid;
        int direction = 1;
        double adjustCoefficient = 1.0;
        double multDeviatorPressureCDDeform = 1.4;
        double multDeviatorPressureCDProchn = 1.0;
        double multDeviatorPressureNNProchn = 1.0;
        double multDeviatorPressureCNProchn = 1.0;
        Measurements::Force friction = Measurements::Force::fromNewtons(10);
        Measurements::Speed adjustSpeed = Measurements::Speed::fromMillimetresPerMinute(1.0);
        Measurements::Force maxForce = Measurements::Force::fromNewtons(9500);
        /**
         * @brief nameUnit      "loadFrame", "shearDevice", "cellVolumeter", "poreVolumeter"
         */
        QString nameUnit;
        Unit(QString name): nameUnit(name) {}
    };

    QMap<QString, Unit*> m_units;

    Unit *unit(QString nameUnit) {
        if (m_units.contains(nameUnit))
            return m_units[nameUnit];
        return nullptr;
    }

    InstrumentParameters() {
        QString unit = "loadFrame";
        m_units.insert(unit, new Unit(unit));
        unit = "shearDevice";
        m_units.insert(unit, new Unit(unit));
        unit = "cellVolumeter";
        m_units.insert(unit, new Unit(unit));
        unit = "poreVolumeter";
        m_units.insert(unit, new Unit(unit));
    }
};




class SettingsManager : public QObject
{
    Q_OBJECT

public:
    static SettingsManager *instance() {
        static SettingsManager sm;
        return &sm;
    }

    bool load();
    bool save();


    int timeout() const { return m_timeoutMs; }
    void setTimeout(int timeout) { m_timeoutMs = timeout; }

    int timeoutCount() const { return m_timeoutCount; }
    void setTimeoutCount(int count) { m_timeoutCount = count; }

    int idleRestoreTime() const { return m_idleRestoreTime; }
    void setIdleRestoreTime(int time) { m_idleRestoreTime = time; }
    \
    int maxCritetionCount() const { return m_maxCriterionCount; }
    void setMaxCritetionCount(int count) { m_maxCriterionCount = count; }

    QString dataPath() const { return m_dataPath; }
    void setDataPath(const QString &dataPath) { m_dataPath = dataPath; }

    QString databasePath() const { return m_databasePath; }
    void setDatabasePath(const QString &databasePath) { m_databasePath = databasePath; }

    QString archivePath() const { return m_archivePath; }
    void setArchivePath(const QString &archivePath) { m_archivePath = archivePath; }

    QString reportPath() const { return m_reportPath; }
    void setReportPath(const QString &reportPath) { m_reportPath = reportPath; }

    QString templatePath() const { return m_templatePath; }
    void setTemplatePath(const QString &templatePath) { m_templatePath = templatePath; }

    QString restorePath() const { return m_restorePath; }
    void setRestorePath(const QString &restorePath) { m_restorePath = restorePath; }

    QMap <QString, QVariant> getServers() const { return m_addressDevices; }
    void setServers(QMap <QString, QVariant> map) { m_addressDevices = map; }

    InstrumentParameters *instrumentParameters(const QString &name) {
        if (!m_instrumentParameters.contains(name)) {
            m_instrumentParameters.insert(name,  new InstrumentParameters);
        }
        return m_instrumentParameters[name];
    }
    void setInstrumentParameters(const QString &name, InstrumentParameters *parameters) {
        m_instrumentParameters.insert(name, parameters);
    }

    QStringList listInstrumentAddresses();
    QStringList listAddressDevices() { return m_addressDevices.keys(); }
    void removeDevice(QString name);
private:
    SettingsManager() = default;

    int m_timeoutMs = 250;
    int m_timeoutCount = 3;
    int m_idleRestoreTime = 30;

    int m_maxCriterionCount = 3;

    QString m_dataPath;
    QString m_databasePath;
    QString m_archivePath;
    QString m_reportPath;
    QString m_templatePath;
    QString m_restorePath;

    QMap <QString, QVariant> m_addressDevices;        // Address, SN

    QMap <QString, InstrumentParameters*> m_instrumentParameters;

};

#endif // SETTINGSMANAGER_H
