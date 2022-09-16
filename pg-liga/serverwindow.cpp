#include "serverwindow.h"
#include "ui_serverwindow.h"
#include <QDebug>


ServerWindow::ServerWindow(QString host_, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerWindow)
{
    ui->setupUi(this);

    qDebug() << host_;
    clnt = new ClientManager(host_, this);
    connect(clnt, &ClientManager::connectedClient, this, &ServerWindow::clientConnected, Qt::QueuedConnection);
    connect(clnt, &ClientManager::disconnectClient, this, &ServerWindow::onDisconnectClient, Qt::QueuedConnection);
    connect(clnt, &ClientManager::readyReadResponse, this, &ServerWindow::onReadyReadResponse, Qt::QueuedConnection);
    timerPollSensors.setInterval(1000);
    connect(&timerPollSensors, &QTimer::timeout, this, &ServerWindow::onPollSensorsCurrentPage);
//    QTimer::singleShot(300, clnt, &ClientManager::connectToHost);
    setEnableWidget(false);
}

ServerWindow::~ServerWindow()
{
    delete ui;
}

void ServerWindow::on_readBtn()
{
    qDebug() << "read";
    fflush(stderr);
}

void ServerWindow::onDisconnectClient()
{
    ui->textEditLogout->append("Client disconnected:\n" + clnt->clientLastError);
    setEnableWidget(false);
}

void ServerWindow::clientConnected()
{
    ui->textEditLogout->append("Client connected:");
    qDebug() << Q_FUNC_INFO;
    on_updateClientExperiments_clicked();
    setEnableWidget(true);
}

void ServerWindow::onReadyReadResponse(const QJsonObject &jresponse)
{
    if (jresponse["type"].toString() == "manager") {
        if (jresponse.contains("get")) {
            qDebug() << "count connected exeperiments =" << jresponse["clients_size"].toString();
            QJsonArray jarr;
            jarr = jresponse["clients_address"].toArray();
            QLayoutItem *item;
            while ((item = ui->verticalLayout->takeAt(0)) != nullptr)
                delete item;
            for (const auto &item: qAsConst(jarr)) {
                qDebug() << "addr " << item.toString();
                QPushButton *btn = new QPushButton("INSTR:" + item.toString(), this);
                ui->verticalLayout->addWidget(btn);
                connect(btn, &QPushButton::clicked, this, &ServerWindow::clickBtnNumInstr);
            }
            jServerConfig = QJsonDocument::fromJson(QByteArray::fromBase64(jresponse["config"].toString().toUtf8())).object();
            ui->textEditLogout->append(QJsonDocument(jServerConfig).toJson());
        }
    }
    else if (jresponse["type"].toString() == "client") {
        QString instr = "INSTR:" + jresponse["address"].toString();
        auto *tab = openTabs.value(instr, nullptr);
        if (tab == nullptr) {
            qDebug() << Q_FUNC_INFO << "for incomming message, instr not found";
        } else {
            tab->onReadyResponse(jresponse);
        }
    }

}

void ServerWindow::clickBtnNumInstr()
{
    QPushButton *btn = qobject_cast<QPushButton*>(this->sender());
    auto *tab = openTabs.value(btn->text(), nullptr);
    if (tab == nullptr) {
        if (openTabs.isEmpty())
            timerPollSensors.start();
        ExperimentView *expView =  new ExperimentView(this);
        connect(expView, &ExperimentView::sendRequest, this, &ServerWindow::on_sendRequest);
        openTabs.insert(btn->text(), expView);
        ui->tabWidget->addTab(expView, btn->text());
        ui->tabWidget->setCurrentWidget(expView);
    } else {
        ui->tabWidget->setCurrentWidget(tab);
    }
    qDebug() << btn->text();
}

void ServerWindow::onPollSensorsCurrentPage()
{

    if (ui->tabWidget->currentIndex() == 0) {
//        qDebug() << "current server page";
    } else {
        QString srch = openTabs.key(qobject_cast<ExperimentView *>(ui->tabWidget->currentWidget()), "");
        QJsonObject jobj;
        jobj["type"] = "client";
        jobj["CMD"] = "read_sensors";
        jobj["address"] = srch.split(':')[1];
        clnt->sendReadyRequest(jobj);
    }
}

void ServerWindow::on_sendRequest(QJsonObject &jobj)
{
    ExperimentView *wgt = qobject_cast<ExperimentView *>(this->sender());
    QString srch = openTabs.key(wgt, "");
    if (srch != "") {
        jobj["address"] = srch.split(':')[1];
        clnt->sendReadyRequest(jobj);
    } else {
        qDebug() << Q_FUNC_INFO << "widget experiment not found";
    }
}

void ServerWindow::on_readBtn_clicked()
{
    qDebug() << Q_FUNC_INFO;
}

void ServerWindow::on_updateClientExperiments_clicked()
{
    QJsonObject jobj;
    jobj["type"] = "manager";
    jobj["get"] = "settings";
    clnt->sendReadyRequest(jobj);
}

void ServerWindow::on_btnCloseServerWindow_clicked()
{
    this->deleteLater();
}

void ServerWindow::setEnableWidget(bool enable)
{
    ui->groupBox_3->setEnabled(enable);
    ui->groupBox->setEnabled(enable);
    for (auto it: qAsConst(openTabs))
        it->setEnabled(enable);
}

void ServerWindow::on_pushButton_clicked()
{
    QJsonObject jobj;
    jobj["type"] = "manager";
    jobj["stop"] = "manager";
    clnt->sendReadyRequest(jobj);
}

void ServerWindow::on_pushButton_2_clicked()
{
    QJsonObject jobj;
    jobj["type"] = "manager";
    jobj["CMD"] = "stop_modbus";
    clnt->sendReadyRequest(jobj);
}

void ServerWindow::on_btnStartModbus_clicked()
{
    QJsonObject jobj;
    jobj["type"] = "manager";
    jobj["CMD"] = "start_modbus";
    clnt->sendReadyRequest(jobj);
}

void ServerWindow::on_btnAddInstr_clicked()
{
    QJsonObject jobj;
    jobj["type"] = "manager";
    jobj["CMD"] = "update_config";

    int addr = ui->spinNumInstr->value();
    QString prog_name = QString("experiment-%1").arg(addr);
    QJsonArray jprograms = jServerConfig["programs"].toArray();
    for (auto item: qAsConst(jprograms))
        if (item.toObject()["name"].toString() == prog_name)
            return;
    QJsonObject jInstr;
    jInstr["program"] = "experiment";
    jInstr["name"] = prog_name;
    jInstr["path"] = QString("./experiments/%1").arg(addr);
    jprograms.append(jInstr);
    jServerConfig["programs"] = jprograms;

    jobj["settings"] = QString(QJsonDocument(jServerConfig).toJson().toBase64());
    clnt->sendReadyRequest(jobj);
    QTimer::singleShot(1000, this, &ServerWindow::on_updateClientExperiments_clicked);
}

void ServerWindow::on_btnRemoveInstr_clicked()
{
    QJsonObject jobj;
    jobj["type"] = "manager";
    jobj["CMD"] = "update_config";

    int addr = ui->spinNumInstr->value();
    QString prog_name = QString("experiment-%1").arg(addr);
    QJsonArray jprograms = jServerConfig["programs"].toArray();
    for (auto it = jprograms.begin(); it != jprograms.end(); it++) {
        if ((*it).toObject()["name"].toString() == prog_name) {
            jprograms.erase(it);
            break;
        }
    }
    jServerConfig["programs"] = jprograms;

    jobj["settings"] = QString(QJsonDocument(jServerConfig).toJson().toBase64());
    clnt->sendReadyRequest(jobj);
    QTimer::singleShot(1000, this, &ServerWindow::on_updateClientExperiments_clicked);
}
