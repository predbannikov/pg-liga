#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QMap>
#include <QObject>

#include "physics/measurements.h"

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

    int maxAddress() const { return m_maxAddress; }
    void setMaxAddress(int maxAddress) { m_maxAddress = maxAddress; }

    int pollPeriod() const { return m_pollPeriodMs; }
    void setPollPeriod(int pollPeriod) { m_pollPeriodMs = pollPeriod; }

    int timeout() const { return m_timeoutMs; }
    void setTimeout(int timeout) { m_timeoutMs = timeout; }

    int timeoutCount() const { return m_timeoutCount; }
    void setTimeoutCount(int count) { m_timeoutCount = count; }

    bool avoidIdenticalRequest() const { return m_avoidIdenticalRequest; }
    void enableAvoidIdenticalRequest(bool enable) { m_avoidIdenticalRequest = enable; }

    int idleRestoreTime() const { return m_idleRestoreTime; }
    void setIdleRestoreTime(int time) { m_idleRestoreTime = time; }
    \
    int maxCritetionCount() const { return m_maxCriterionCount; }
    void setMaxCritetionCount(int count) { m_maxCriterionCount = count; }

    bool autoStart() const { return m_enableAutoStart; }
    void enableAutoStart(bool enable) { m_enableAutoStart = enable; }

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

    bool getEnableDumpingProtocol() const { return m_enableDumpingProtocol; }
    void setEnableDumpingProtocol(bool enable) { m_enableDumpingProtocol = enable; }

    QString getServers() const { return servers; }
    void setServers(QString str) {servers = str;};

private:
    SettingsManager() = default;

    int m_maxAddress = 3;
    int m_pollPeriodMs = 1000;
    int m_timeoutMs = 250;
    int m_timeoutCount = 3;
    int m_idleRestoreTime = 30;
    bool m_avoidIdenticalRequest = false;

    int m_maxCriterionCount = 3;

    QString m_dataPath;
    QString m_databasePath;
    QString m_archivePath;
    QString m_reportPath;
    QString m_templatePath;
    QString m_restorePath;

    bool m_enableAutoStart = true;

    QString servers;

    bool m_enableDumpingProtocol = false;
};

#endif // SETTINGSMANAGER_H
