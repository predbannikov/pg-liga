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
class ClientWindow;
}

class ClientWindow : public QWidget
{
    Q_OBJECT
    QJsonObject jServerConfig;
public:
    explicit ClientWindow(QString host_, QWidget *parent = nullptr);
    ~ClientWindow();

public slots:
    void on_readBtn();
    void clientConnected();
    void clickBtnNumInstr();
private slots:
    void onPollSensorsCurrentPage();
    void onDisconnectClient();


    void on_sendRequest(QJsonObject &jobj);
//    void on_readyResponse(QJsonObject &jobj);

    void on_tabWidget_tabCloseRequested(int index);

    void on_pushButton_3_clicked();

private:

    Ui::ClientWindow *ui;
    ClientManager *clnt = nullptr;
    ExperimentView *experimentView = nullptr;
    QTimer timerPollSensors;
};

#endif // SERVERWINDOW_H
