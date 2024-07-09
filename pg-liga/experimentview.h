#ifndef EXPERIMENTVIEW_H
#define EXPERIMENTVIEW_H

#include <QWidget>
#include <QJsonObject>
#include <QLineEdit>
#include <QLabel>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

#include "plotadapter.h"
#include "experimentdata.h"
#include "decoratedplot.h"

#include "datastore/datastore.h"

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

public slots:
    void onReadyResponse(const QJsonObject &jobj);
    void onReadDataStore();

private slots:
    void onCreateJsonObject();


    void onReadSensors();

    void on_comboBox_activated(const QString &arg1);

    void on_dateTimeEdit_timeChanged(const QTime &time);

    void on_btnAddStep_clicked();

    void on_pushButton_2_clicked();

    void on_btnStart_clicked();

    void on_btnClearLogOut_clicked();

    void on_btnClearTextEdit_clicked();

    void on_btnMoveUp_clicked();

    void on_btnMoveDown_clicked();

    void on_btnStopStepper_clicked();

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

signals:
    void sendRequest(QJsonObject &jobj);

private:
    void setupPlots();
    void clearData();

    QMap<QString, DataStore> dataStore;



    QMap<QString, ExperimentData*> m_deformData;
    ExperimentData *dataDeform = nullptr;
    DecoratedPlot *deformVsTime = nullptr;

    QMap<QString, ExperimentData*> m_presureData;
    ExperimentData *dataPressure = nullptr;
    DecoratedPlot *pressureVsTime = nullptr;


    QMap<QString, ExperimentData*> m_positionData;
    ExperimentData *dataPosition = nullptr;
    DecoratedPlot *positionVsTime = nullptr;


    QList <QLineEdit*> steps;
    QJsonObject jRequest;
    Ui::ExperimentView *ui;
    qint64 timeInterval(const QString &date, const QString &format);
    QTimer timerIntervalUpdate;
};

#endif // EXPERIMENTVIEW_H
