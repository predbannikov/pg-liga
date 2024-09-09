#ifndef EXPERIMENTVIEW_H
#define EXPERIMENTVIEW_H

/**
 *      Логика состояния соединения:
 *          ClientWindow после подключение к серверу, получает сигнал и вызывает clientSetConnectedState(state)
 *          При создании ExperimentView в конструкторе запускается clientSetConnectedState(false)
 *          Дальше состояние соединения управляется ClientWindow
 *
 *      Логика управляющих кнопок запуска, останова, сброса и продолжения эксперимента:
 *          При создании ExperimentView в конструкторе
 *
 *
 */

#include <QWidget>
#include <QJsonObject>
#include <QLineEdit>
#include <QLabel>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QTimer>
#include <QScrollArea>
#include <QMenu>
#include <QAction>
#include <QIcon>

#include "customgraph.h"
#include "steppedmodeleditor.h"
#include "operationactions.h"
#include "controlpanelwgt.h"

//#include "plotadapter.h"
//#include "experimentdata.h"
//#include "decoratedplot.h"

#include "datastore.h"

namespace Strings {

static const auto calibrate = QObject::tr("Тарировать");
static const auto zero = QObject::tr("Обнулить");
static const auto reset = QObject::tr("Сбросить ноль");

static const auto force = QObject::tr("усилие");
static const auto deform = QObject::tr("деформация");
static const auto pressure = QObject::tr("давление");
static const auto position = QObject::tr("положение");

static const auto kPa = QObject::tr("кПа");
static const auto kN = QObject::tr("кН");
static const auto N = QObject::tr("H");
static const auto mm = QObject::tr("мм");
static const auto mm3 = QObject::tr("мм³");
static const auto cm3 = QObject::tr("см³");
static const auto percent = QObject::tr("%");

static const auto mmMin = QObject::tr("мм/мин");
static const auto kPaSec = QObject::tr("кПа/с");

static const auto sigma = QObject::tr("σ");
static const auto epsilon = QObject::tr("ε");

}


namespace Ui {
class ExperimentView;
}

class ExperimentView : public QWidget
{
    Q_OBJECT

public:
    explicit ExperimentView(QWidget *parent = nullptr);
    ~ExperimentView();

    void clientSetConnectedState(bool state);

public slots:
    void onReadyResponse(const QJsonObject &jobj);
    void onReadDataStore();

    void addOperationActions();
    void updateIndexOperationActions();
    void moveUpOperation();
    void moveDownOperation();



private slots:


    /**
     *  Обновление статуса о плате, датчики, состояния и т.д.
     */
    void onUpdateStatus();



    void initServicePanel();
    void initControlPanel();

    void onCreateJsonObject();


    void on_comboBox_activated(const QString &arg1);

    void on_dateTimeEdit_timeChanged(const QTime &time);

    void on_btnUnlockPid_clicked();

    void on_btnSaveImage_clicked();

    void on_btnGetStoreData_clicked();

    void on_btnGetSensorValue_clicked();

    void on_btnInitStoreData_clicked();

    void on_btnSetSettings_clicked();

    void on_btnSetTarget_clicked();

    void on_btnClearDataStore_clicked();

    void on_btnSetHz_clicked();

    void on_btnStopStoreData_clicked();

    void on_btnHardReset_clicked();

    void on_btnSetPidP_clicked();

    void on_btnSetPID_D_clicked();

    void on_btnSetUpPidP_clicked();

    void on_btnSetUpPID_D_clicked();

    void on_btnSetState_clicked();

    void on_btnLoadFrameMoveUp_clicked();

    void on_btnLoadFrameMoveDown_clicked();

    void on_btnLoadFrameStopStepper_clicked();

    void on_btnTest_clicked();

    void on_btnLoadFrameSensorForceSetZero_clicked();

    void on_btnLoadFrameSensorForceReset_clicked();

    void on_btnLoadFrameSensorDeformSetZero_clicked();

    void on_btnLoadFrameSensorDeformReset_clicked();

    void on_btnVolumetr1SensorPressureSetZero_clicked();

    void on_btnVolumetr1SensorPressureReset_clicked();

signals:
    void sendRequest(QJsonObject &jobj);

private:
    void setupPlots();
    void clearData();


    QMap<QString, DataStore> dataStore;

    QMap<QString, ExperimentData*> m_experimentData;
    DecoratedPlot *deformVsTime = nullptr;

    QMap<QString, ExperimentData*> m_presureData;
    DecoratedPlot *pressureVsTime = nullptr;


    QMap<QString, ExperimentData*> m_positionData;
    DecoratedPlot *positionVsTime = nullptr;

    CustomGraph *customPlot = nullptr;


    QList <QLineEdit*> steps;
    QJsonObject jRequest;
    Ui::ExperimentView *ui;
    qint64 timeInterval(const QString &date, const QString &format);
    QTimer *timerUpdateDataStore = nullptr;
    QTimer *timerUpdateStatus = nullptr;
};

#endif // EXPERIMENTVIEW_H
