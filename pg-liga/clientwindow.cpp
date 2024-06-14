#include "clientwindow.h"
#include "ui_clientwindow.h"
#include <QDebug>
#include "mainwindow.h"


#define SENSORS_POLLING_INTERVAL_TIMEOUT    1000


ClientWindow::ClientWindow(QString host_, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerWindow)
{
    ui->setupUi(this);

    qDebug() << host_;
    clnt = new ClientManager(host_, this);
    connect(clnt, &ClientManager::connectedClient, this, &ClientWindow::clientConnected, Qt::QueuedConnection);
    connect(clnt, &ClientManager::disconnectClient, this, &ClientWindow::onDisconnectClient, Qt::QueuedConnection);
    timerPollSensors.setInterval(SENSORS_POLLING_INTERVAL_TIMEOUT);
    connect(&timerPollSensors, &QTimer::timeout, this, &ClientWindow::onPollSensorsCurrentPage);
    this->resize(720, 1280);
//    ui->verticalLayout->setSizeConstraint(QLayout::SetNoConstraint);
    setEnableWidget(false);
    QList<int> sizes;
    sizes << 100 << 0;

    experimentView = new ExperimentView(this);
    connect(experimentView, &ExperimentView::sendRequest, this, &ClientWindow::on_sendRequest);
    connect(clnt, &ClientManager::readyReadResponse, experimentView, &ExperimentView::onReadyResponse, Qt::QueuedConnection);
    ui->tabWidget->addTab(experimentView, "Стадия номер 1");
//    ui->splitter->setSizes(sizes);
}

ClientWindow::~ClientWindow()
{
    delete ui;
}

void ClientWindow::on_readBtn()
{
    qDebug() << "read";
    fflush(stderr);
}

void ClientWindow::onDisconnectClient()
{
//    ui->textEditLogout->append("Client disconnected:\n" + clnt->clientLastError);
    setEnableWidget(false);
}

void ClientWindow::clientConnected()
{
//    ui->textEditLogout->append("Client connected:");
    qDebug() << Q_FUNC_INFO;
    setEnableWidget(true);
}

void ClientWindow::onReadyReadResponse(const QJsonObject &jresponse)
{
//    qDebug().noquote() << jresponse;
//    if (jresponse["type"].toString() == "manager") {
//        if (jresponse.contains("get")) {
//            qDebug() << "count connected exeperiments =" << jresponse["clients_size"].toString();
//            jServerConfig = QJsonDocument::fromJson(QByteArray::fromBase64(jresponse["config"].toString().toUtf8())).object();
//            ui->textEditLogout->append(QJsonDocument(jServerConfig).toJson());
//            QJsonArray jarr;
//            jarr = jresponse["clients_address"].toArray();
//            QLayoutItem *item;
//            while ((item = ui->verticalLayout->takeAt(0)) != nullptr) {
//                qDebug() << "remove" << qobject_cast<InstrumentButton*>((*item).widget())->text();
//                ui->verticalLayout->removeItem(item);
//                item->widget()->deleteLater();
//                delete item;
//            }
//            ui->verticalLayout->update();
//            for (const auto &item: qAsConst(jarr)) {
//                qDebug() << "addr " << item.toString();
//                InstrumentButton *instrBtn = new InstrumentButton(name, this);
//                instrBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
////                instrBtn->resize(150, 128);
////                instrBtn->setText("INSTR:" + item.toString());
////                QPushButton *btn = new QPushButton("INSTR:" + item.toString(), this);
////                for (auto &item: ui->verticalLayout->children()) {
////                    QPushButton *childBtn = qobject_cast<QPushButton *>(item);
////                    if (childBtn != nullptr)
////                        if (childBtn->text() == btn->text()

////                }
//                ui->verticalLayout->addWidget(instrBtn);
//                connect(instrBtn, &InstrumentButton::clicked, this, &ServerWindow::clickBtnNumInstr);
//            }
//        }
//    }
//    else if (jresponse["type"].toString() == "client") {
//        QString instr = "INSTR:" + jresponse["address"].toString();
//        auto *tab = openTabs.value(instr, nullptr);
//        if (tab == nullptr) {
//            qDebug() << Q_FUNC_INFO << "for incomming message, instr not found";
//        } else {
//            tab->onReadyResponse(jresponse);
//        }
//    }

}

void ClientWindow::clickBtnNumInstr()
{
    InstrumentButton *btn = qobject_cast<InstrumentButton*>(this->sender());
    QString instrName = btn->getSN();
    auto *tab = openTabs.value(instrName, nullptr);
    if (tab == nullptr) {
        if (openTabs.isEmpty())
            timerPollSensors.start();
        ExperimentView *expView =  new ExperimentView(this);
        connect(expView, &ExperimentView::sendRequest, this, &ClientWindow::on_sendRequest);
        openTabs.insert(instrName, expView);
        ui->tabWidget->addTab(expView, instrName);
        ui->tabWidget->setCurrentWidget(expView);
    } else {
        ui->tabWidget->setCurrentWidget(tab);
    }
    qDebug() << btn->text();
}

void ClientWindow::onPollSensorsCurrentPage()
{

    if (ui->tabWidget->currentIndex() == 0) {
//        qDebug() << "current server page";
    } else {
        auto experView = qobject_cast<ExperimentView *>(ui->tabWidget->currentWidget());
        QString srch = openTabs.key(experView, "");
        if (srch == "") {
            qDebug() << "text is empty";
            return;
        }
        experView->onReadDataStore();

//        QJsonObject jobj;
//        jobj["type"] = "client";
//        jobj["CMD"] = "read_sensors";
//        jobj["address"] = srch.split(':')[1];
//        clnt->sendReadyRequest(jobj);
    }
}

void ClientWindow::on_sendRequest(QJsonObject &jobj)
{
    qDebug() << jobj;
    clnt->sendReadyRequest(jobj);
}

void ClientWindow::setEnableWidget(bool enable)
{
//    ui->scrollArea->setEnabled(enable);
    for (auto it: qAsConst(openTabs))
        it->setEnabled(enable);
}

void ClientWindow::on_tabWidget_tabCloseRequested(int index)
{
    this->deleteLater();
}

void ClientWindow::on_pushButton_3_clicked()
{
//    ui->textEditLogout->clear();
}
