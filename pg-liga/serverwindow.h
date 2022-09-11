#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include "experimentview.h"

#include "clientmanager.h"

namespace Ui {
class ServerWindow;
}

class ServerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ServerWindow(QString host_, QWidget *parent = nullptr);
    ~ServerWindow();

public slots:
    void on_readBtn();
    void clientConnected();
    void onReadyReadResponse(const QJsonObject &jresponse);
    void clickBtnNumInstr();
private slots:
    void onPollSensorsCurrentPage();
    void onDisconnectClient();


    void on_sendRequest(QJsonObject &jobj);
//    void on_readyResponse(QJsonObject &jobj);

    void on_readBtn_clicked();

    void on_updateClientExperiments_clicked();

    void on_btnCloseServerWindow_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:

    void setEnableWidget(bool enable);

    Ui::ServerWindow *ui;
    QMap <QString, ExperimentView*> openTabs;
    ClientManager *clnt = nullptr;
    QTimer timerPollSensors;
};

#endif // SERVERWINDOW_H
