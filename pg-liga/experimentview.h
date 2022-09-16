#ifndef EXPERIMENTVIEW_H
#define EXPERIMENTVIEW_H

#include <QWidget>
#include <QJsonObject>
#include <QLineEdit>
#include <QLabel>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

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

private slots:
    void onCreateJsonObject();
    void onReadSensors();

    void on_pushButton_clicked();

    void on_spinPressure_textChanged(const QString &arg1);

    void on_comboBox_activated(const QString &arg1);

    void on_spinCriter_textChanged(const QString &arg1);

    void on_dateTimeEdit_timeChanged(const QTime &time);

    void on_btnSendRequest_clicked();

    void on_btnAddStep_clicked();

    void on_pushButton_2_clicked();

    void on_btnSendConfig_clicked();

    void on_btnReadConfig_clicked();

    void on_btnStart_clicked();

    void on_btnClearLogOut_clicked();

    void on_btnCloseWindow_clicked();

    void on_btnGetProtocol_clicked();

    void on_btnClearTextEdit_clicked();

    void on_btnMoveUp_clicked();

    void on_btnMoveDown_clicked();

    void on_btnStopStepper_clicked();

    void on_btnUnlockPid_clicked();

    void on_btnCloseInstr_clicked();

signals:
    void sendRequest(QJsonObject &jobj);

private:
    QList <QLineEdit*> steps;
    QJsonObject jRequest;
    Ui::ExperimentView *ui;
    qint64 timeInterval(const QString &date, const QString &format);
};

#endif // EXPERIMENTVIEW_H
