#include "settingsmanager.h"

#include <QDir>
#include <QSettings>
#include <QCoreApplication>

static const char settingsFileName[] = "settings.ini";
/* Groups */
static const char networkGroup[] = "Network";
static const char pathsGroup[] = "Paths";
static const char instrumentGroup[] = "Instrument";
static const char experimentGroup[] = "Experiment";
static const char miscGroup[] = "Misc";
/* Network */
static const char maxAddrKey[] = "maxAddress";
static const char pollPeriodKey[] = "pollPeriod";
static const char timeoutKey[] = "timeout";
static const char timeoutCountKey[] = "timeoutCountKey";
static const char avoidIdenticalRequestKey[] = "avoidIdenticalRequestKey";    // Признак "игнорировать запрос на чтение, если такой же запрос уже есть в очереди"

static const char enableAutoStartKey[] = "enableAutoStart";
static const char enableIdleRestoreKey[] = "enableIdleRestore";
static const char idleRestoreTimeKey[] = "idleRestoreTime";
/* Path */
static const char testDataPathKey[] = "testDataPath";
static const char dataBasePathKey[] = "dataBasePath";
static const char archivePathKey[] = "archivePath";
static const char reportPathKey[] = "reportPath";
static const char templatePathKey[] = "templatePath";
static const char restorePathKey[] = "restorePath";
/* Instrument */
static const char instrumentTypeKey[] = "instrumentType";

static const char triaxialChamberTypeKey[] = "triaxialChamberType";
static const char triaxialPistonDiameterKey[] = "triaxialPistonDiameter";
static const char pistonWeightKey[] = "pistonWeight";

static const char loadFrameFrictionKey[] = "loadFrameFriction";
static const char shearDeviceFrictionKey[] = "shearDeviceFriction";

static const char loadFrameScalingKey[] = "loadFrameScaling";
static const char shearDeviceScalingKey[] = "shearDeviceScaling";

static const char loadFrameAdjustSpeedKey[] = "loadFrameAdjustSpeed";
static const char loadFrameAdjustCoefficientKey[] = "loadFrameAdjustCoefficient";

static const char loadFrameStepDistanceKey[] = "loadFrameStepDistance";
static const char shearDeviceStepDistanceKey[] = "shearDeviceStepDistance";

static const char volumeterMultiplicationCoefficientKey[] = "volumeterMultiplicationCoefficient";
/* Experiment */
static const char maxCriterionCountKey[] = "maxCritetionCount";
static const char relaxationExperimentKey[] = "relaxationExperimentEnabled";
static const char enableReadlAllSensorsKey[] = "enableReadAllSensors";
static const char specimenChangeKey[] = "specimenChange";

/* Misc&&Service */
static const char enableTipsKey[] = "enableTips";
static const char enableDumpingProtocol[] = "enableDumpingProtocol";
static const char serversConf[]= "servers";
static const char enablePasswordsKey[] = "enablePasswords";
static const char enableServiceModeKey[] = "enableServiceMode";


bool SettingsManager::load()
{
    const auto fileName = QDir(qApp->applicationDirPath()).filePath(settingsFileName);
    if(!QFile(fileName).exists()) {
        return false;
    }

    QSettings settings(fileName, QSettings::IniFormat);

    settings.beginGroup(networkGroup);
    if (settings.contains(serversConf)) {
        m_addressDevices = settings.value(serversConf).toMap();
    }

    if(settings.contains(timeoutKey)) {
        m_timeoutMs = settings.value(timeoutKey).toInt();
    }

    if(settings.contains(timeoutCountKey)) {
        m_timeoutCount = settings.value(timeoutCountKey).toInt();
    }

    if(settings.contains(idleRestoreTimeKey)) {
        m_idleRestoreTime = settings.value(idleRestoreTimeKey).toInt();
    }
    settings.endGroup();

    settings.beginGroup(pathsGroup);
    if(settings.contains(testDataPathKey)) {
        m_dataPath = settings.value(testDataPathKey).toString();
    }

    if(settings.contains(dataBasePathKey)) {
        m_databasePath = settings.value(dataBasePathKey).toString();
    }

    if(settings.contains(archivePathKey)) {
        m_archivePath = settings.value(archivePathKey).toString();
    }

    if(settings.contains(reportPathKey)) {
        m_reportPath = settings.value(reportPathKey).toString();
    }

    if(settings.contains(templatePathKey)){
        m_templatePath = settings.value(templatePathKey).toString();
    }

    if(settings.contains(restorePathKey)) {
        m_restorePath = settings.value(restorePathKey).toString();
    }
    settings.endGroup();

    settings.beginGroup(instrumentGroup);
    const auto names = settings.childGroups();
    for(const auto &name : names) {
//        InstrumentParameters parameters;

//        settings.beginGroup(name);

//        if(settings.contains(instrumentTypeKey)) {
//            parameters.instrumentType = settings.value(instrumentTypeKey).toInt();
//        }

//        if(settings.contains(triaxialChamberTypeKey)) {
//            parameters.triaxialChamber.type = settings.value(triaxialChamberTypeKey).toInt();
//        }

//        if(settings.contains(triaxialPistonDiameterKey)) {
//            parameters.triaxialChamber.pistonDiameter = Measurements::Length::fromMillimetres(settings.value(triaxialPistonDiameterKey).toDouble());
//        }

//        if(settings.contains(pistonWeightKey)) {
//            parameters.triaxialChamber.pistonWeight = Measurements::Force::fromNewtons(settings.value(pistonWeightKey).toDouble());
//        }

//        if(settings.contains(loadFrameFrictionKey)) {
//            parameters.loadFrame.friction = Measurements::Force::fromNewtons(settings.value(loadFrameFrictionKey).toDouble());
//        }

//        if(settings.contains(shearDeviceFrictionKey)) {
//            parameters.shearDevice.friction = Measurements::Force::fromNewtons(settings.value(shearDeviceFrictionKey).toDouble());
//        }

//        if(settings.contains(loadFrameScalingKey)) {
//            parameters.loadFrame.scaling = settings.value(loadFrameScalingKey).toDouble();
//        }

//        if(settings.contains(shearDeviceScalingKey)) {
//            parameters.shearDevice.scaling = settings.value(shearDeviceScalingKey).toDouble();
//        }

//        if(settings.contains(loadFrameAdjustSpeedKey)) {
//            parameters.loadFrame.adjustSpeed = Measurements::Speed::fromMillimetresPerMinute(settings.value(loadFrameAdjustSpeedKey).toDouble());
//        }

//        if(settings.contains(loadFrameAdjustCoefficientKey)) {
//            parameters.loadFrame.adjustCoefficient = settings.value(loadFrameAdjustCoefficientKey).toDouble();
//        }

//        if(settings.contains(loadFrameStepDistanceKey)) {
//            parameters.loadFrame.stepDistance = Measurements::Length::fromMicrometres(settings.value(loadFrameStepDistanceKey).toDouble());
//        }

//        if(settings.contains(shearDeviceStepDistanceKey)) {
//            parameters.shearDevice.stepDistance = Measurements::Length::fromMicrometres(settings.value(shearDeviceStepDistanceKey).toDouble());
//        }

//        if(settings.contains(volumeterMultiplicationCoefficientKey)) {
//            parameters.cellVolumeter.multipicationCoefficient = settings.value(volumeterMultiplicationCoefficientKey).toDouble();
//            parameters.poreVolumeter.multipicationCoefficient = settings.value(volumeterMultiplicationCoefficientKey).toDouble();
//        }

//        settings.endGroup();

    }
    settings.endGroup();

    settings.beginGroup(experimentGroup);

    if(settings.contains(maxCriterionCountKey)) {
        m_maxCriterionCount = settings.value(maxCriterionCountKey).toInt();
    }
    settings.endGroup();

    settings.beginGroup(miscGroup);

    settings.endGroup();

    return (settings.status() == QSettings::NoError);
}

bool SettingsManager::save()
{
    const auto fileName = QDir(qApp->applicationDirPath()).filePath(settingsFileName);
    QSettings settings(fileName, QSettings::IniFormat);

    settings.beginGroup(networkGroup);
    settings.setValue(serversConf, m_addressDevices);
    settings.setValue(timeoutKey, m_timeoutMs);
    settings.setValue(timeoutCountKey, m_timeoutCount);
    settings.setValue(idleRestoreTimeKey, m_idleRestoreTime);
    settings.endGroup();

    settings.beginGroup(pathsGroup);
    settings.setValue(testDataPathKey, m_dataPath);
    settings.setValue(dataBasePathKey, m_databasePath);
    settings.setValue(archivePathKey, m_archivePath);
    settings.setValue(reportPathKey, m_reportPath);
    settings.setValue(templatePathKey, m_templatePath);
    settings.setValue(restorePathKey, m_restorePath);
    settings.endGroup();

    settings.beginGroup(instrumentGroup);


    settings.endGroup();

    settings.beginGroup(experimentGroup);

    settings.setValue(maxCriterionCountKey, m_maxCriterionCount);
    settings.endGroup();

    settings.beginGroup(miscGroup);
    settings.endGroup();

    settings.sync();

    return (settings.status() == QSettings::NoError);
}

QStringList SettingsManager::listInstrumentAddresses()
{
    return m_instrumentParameters.keys();
}

void SettingsManager::removeDevice(QString name)
{
    m_addressDevices.remove(name);
}
