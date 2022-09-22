#include <QMessageBox>
#include <QDateTime>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const auto isOperational = SettingsManager::instance()->load();
    if(!isOperational) {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("<p><b>Не удалось загрузить настройки.</b></p>"
                                "<p>Перейдите в \"Система->Настройки\" и установите необходимые параметры, "
                                "после чего перезапустите программу.</p>"));
    }
    connect(ui->actScan, &QAction::triggered, this, &MainWindow::scanInstruments);
//    connect(ui->tabMain, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);
//    this->resize(1280, 768);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

    ui->serverCmbBox->addItem(SettingsManager::instance()->getServers());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::scanInstruments()
{
    ui->actScan->setEnabled(false);
//    QUrl url(SettingsManager::instance()->getServers());
//    qDebug() << url.host();
//    qDebug() << url.port();
    onScanFinished();
//    setFirstTab(new ProgressLabel(this), tr("Поиск устройств"));
//    InterfaceLiga interface(this);

}

void MainWindow::onScanFinished()
{
    ui->actScan->setEnabled(true);

//    this->centralWidget()->layout()->replaceWidget(this, new ServerWindow);



//    this->layout()->addWidget(new ServerWindow(this));
//    this->setCentralWidget(new ServerWindow(this));



}

void MainWindow::onInstrumentSelected(TYPE_INSTRUMENT type)
{
//    auto *tab = m_openTabs.value (instrument, nullptr);

    qInfo() << QDateTime::currentDateTime() << "[instrumentSelected]" << type;

    QWidget *tab = new QWidget(this);

    if (tab == nullptr)
    {
        // если вкладка выбранного инструмента (прибора Лига) ещё не была открыта в интерфейсе

        // проверяем, есть ли по данному прибору незавершённые эксперименты
//        auto experimentToResume = ExperimentManager::instance()->checkForUnfinished(instrument->name());

//        if (experimentToResume.isValid)
//        {
//            // если незавершённые эксперименты есть

//            // пытаемся понять причину, по которой эксперимент был прерван
//            if (instrument->units().loadFrame    ->position() == Measurements::Length::fromMillimetres(0) &&
//                    instrument->units().shearDevice  ->position() == Measurements::Length::fromMillimetres(0) &&
//                    instrument->units().cellVolumeter->position() == Measurements::Length::fromMillimetres(0) &&
//                    instrument->units().poreVolumeter->position() == Measurements::Length::fromMillimetres(0)  )
//            {
//                experimentToResume.restoreReason = ExperimentManager::PowerError;   // guessing based on reading
//            }
//            else experimentToResume.restoreReason = ExperimentManager::CommunicationError;
//            qInfo() << QDateTime::currentDateTime() << instrument->name() << "Restore reason:" << experimentToResume.restoreReason;

//            // выясняем, нужно ли этот эксперимент восcтанавливать
//            // (если режим восстановления автоматический, то сразу нужно, а
//            // если нет, то проводим опрос местного населения)
//            QMessageBox resumeBox;
//            int isRestoring = 0;
//            if (!m_restoreMode)
//            {
//                resumeBox.setText(QString("На приборе найден незавершённый эксперимент.\n\n"
//                                          "Дата эксперимента: \t%1\n"
//                                          "Тип эксперимента: \t%2\n"
//                                          "Этап: \t%3").arg(experimentToResume.date.toString("dd.MM.yyyy hh:mm:ss"),
//                                                            experimentToResume.typeString,
//                                                            experimentToResume.stageName));
//                resumeBox.setInformativeText("Вы желаете продолжить эксперимент?");
//                resumeBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
//                resumeBox.setDefaultButton(QMessageBox::Ok);

//                isRestoring = resumeBox.exec();
//            }
//            else
//            {
//                isRestoring = QMessageBox::Ok;         // позволяет миновать диалог восстановления эксперимента в авторежиме
//            }

//            // дальнейшие действия в зависимости от того, нужно или нет
//            // восставливать эксперимент
//            if(isRestoring == QMessageBox::Ok)
//            {
//                auto *newTab = new ExperimentContainer(instrument, this);
//                m_openTabs.insert(instrument, newTab);
//                m_instrumentsInUse.insert(newTab, instrument);

//                ui->tabMain->addTab(newTab, instrument->name());
//                ui->tabMain->setCurrentWidget(newTab);

//                newTab->setAutoResume(SettingsManager::instance()->autoStart());
//                newTab->autoResumeRequested(&experimentToResume);
//                ExperimentManager::instance()->setValidState(false, instrument->name());

//                auto client = ClientManager::instance()->client (instrument->parseName(instrument->name()).portName);
//                connect (client, &MinibusClient::clientReconnected, newTab, &ExperimentContainer::onClientReconnected);
//            }
//            else
//            {
//                //  Если пользователь отказывается восстановить сессию, удаляем найденные незавершенные эксперементы
//                {
//                    qDebug() << "cancel restore of the found experiment";

//                    const QString deleteFilenameMask = instrument->name() + "-*.xml";
//                    const QString deleteFilenameMaskLinux = instrument->name() + "-*.xml";
//                    const QString deleteFilenameMaskLinuxAlt = instrument->name() + "-*.xml";

//                    //QDir current;
//                    QDir path;
//                    path.setPath(SettingsManager::instance()->restorePath());
//                    path.setNameFilters({deleteFilenameMask, deleteFilenameMaskLinux,
//                                         deleteFilenameMaskLinuxAlt});
//                    path.setFilter(QDir::Files);
//                    QFileInfoList infoList = path.entryInfoList();

//                    for (const auto &file: qAsConst(infoList)) {
//                        qDebug() << "remove -" << file.absoluteFilePath();
//                        path.remove(file.absoluteFilePath());
//                    }
//                    /// ATT! experimentToResume тут будет удалён (так как пользователь отказался от восстановления
//                    if (!ExperimentManager::instance()->remove_experiment(instrument->name()))
//                        qDebug() << QString("Error: m_experiments not removed %1").arg(instrument->name());
//                }
//                auto *newTab = new ExperimentContainer(instrument, this);
//                m_openTabs.insert(instrument, newTab);
//                m_instrumentsInUse.insert(newTab, instrument);

//                ui->tabMain->addTab(newTab, instrument->name());
//                ui->tabMain->setCurrentWidget(newTab);
//            }
//        }
//        else
//        {
//            // иначе, если незавершённых экспериментов нет

//            auto *newTab = new ExperimentContainer(instrument, this);
//            m_openTabs.insert(instrument, newTab);
//            m_instrumentsInUse.insert(newTab, instrument);

//            ui->tabMain->addTab(newTab, instrument->name());
//            ui->tabMain->setCurrentWidget(newTab);
//        }
    }
    else
    {
//        ui->tabMain->setCurrentWidget(tab);
    }

//    if (m_idleTimer->isActive()) m_idleTimer->stop();



}

void MainWindow::on_actExit_triggered()
{
    qDebug() << "on_actExit_triggered";
    exitWindow();
}

//void MainWindow::on_actScan_triggered()
//{
////    if (checked)
//        scanInstruments();
//}

void MainWindow::on_actSettings_triggered()
{
    SettingsDialog sd(this);
    sd.exec();
}

void MainWindow::exitWindow()
{
    const auto reply = QMessageBox::question(this, tr("Выйти из программы"),
                                             tr("Вы действительно хотите выйти из программы?\n"
                                                "Все незавершенные испытания будут прерваны."));

    if(reply == QMessageBox::Yes) {
//        closeExperimentsEvent();

        qApp->exit();
    }
}

void MainWindow::setFirstTab(QWidget *widget, const QString &title)
{
//   if(ui->tabMain->count() == 0) {
//        ui->tabMain->addTab(widget, title);
//    } else {
//        auto *oldWidget = ui->tabMain->widget(0);
//        ui->tabMain->removeTab(0);
//        ui->tabMain->insertTab(0, widget, title);
//        oldWidget->deleteLater();
//    }
}

void MainWindow::onTabCloseRequested(int idx)
{
    if(idx != 0) {
//        auto *instrument = m_instrumentsInUse.value(ui->tabMain->widget(idx), nullptr);
//        if(instrument) {
//            if(instrument->state() == Instrument::Experiment) {
//                const auto reply = QMessageBox::question(this, tr("Закрыть вкладку"),
//                                                         tr("Вы действительно хотите закрыть вкладку?\n"
//                                                            "Незавершенное испытание на данной установке будет прервано."));
//                if(reply == QMessageBox::Yes) {
//                    instrument->emergencyStop();
//                    closeInstrumentTab(idx);
//                }
//            } else {
//                closeInstrumentTab(idx);
//            }
//        } else {
//            closeInstrumentTab(idx);
//        }

    } else {
        exitWindow();
    }
}

/************************************************
 * 					InstrumentGrid
 * **********************************************/

InstrumentGrid::InstrumentGrid(QWidget *parent):
    QScrollArea(parent),
    m_layout(new FlowLayout(new QWidget(this)))
{
    setWidget(m_layout->parentWidget());
    setWidgetResizable(true);
    setFrameShape(QFrame::NoFrame);
}

void InstrumentGrid::addInstrument(TYPE_INSTRUMENT type)
{
    auto *button = new InstrumentButton("1", type, this);
    connect(button, &QPushButton::clicked, this, &InstrumentGrid::onInstrumentButtonClicked);

    m_layout->addWidget(button);
}

void InstrumentGrid::removeInstrument(TYPE_INSTRUMENT type)
{
    auto *btn = findButton(type);
    if(btn != nullptr) {
        m_layout->removeWidget(btn);
    }
}

void InstrumentGrid::onInstrumentButtonClicked()
{
    auto instrumentButton = qobject_cast<InstrumentButton*>(sender());
    emit instrumentSelected(instrumentButton->instrument());
}

InstrumentButton *InstrumentGrid::findButton(TYPE_INSTRUMENT type)
{
    for(auto *child : children()) {
        if(auto *btn = qobject_cast<InstrumentButton*>(child)) {
//            if(btn->instrument() == instrument) {
//                return btn;
//            }
        }
    }
    return nullptr;
}

/************************************************
 * 					FLOWLAYOUT
 * **********************************************/

FlowLayout::FlowLayout(QWidget *parent):
    QLayout(parent)
{}

FlowLayout::~FlowLayout()
{
    while(QLayoutItem *item = takeAt(0)) {
        delete item;
    }
}

void FlowLayout::addItem(QLayoutItem *item)
{
    m_items.append(item);
}

QSize FlowLayout::sizeHint() const
{
    return minimumSize();
}

QSize FlowLayout::minimumSize() const
{
    return m_minSize;
}

void FlowLayout::setGeometry(const QRect &r)
{
    m_minSize = buildLayout(r);
}

QLayoutItem *FlowLayout::itemAt(int index) const
{
    return m_items.value(index);
}

QLayoutItem *FlowLayout::takeAt(int index)
{
    if ((index >= 0) && (index < m_items.count())) {
        return m_items.takeAt(index);
    } else {
        return nullptr;
    }
}

int FlowLayout::count() const
{

    return m_items.size();
}

QSize FlowLayout::buildLayout(const QRect &r) const
{
    if(m_items.isEmpty()) {

        return QSize();
    }
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);

    const auto effectiveRect = r.adjusted(+left, +top, -right, -bottom);
    const auto itemSizeHint = m_items.first()->sizeHint(); /* Assuming items of identical size */

    const auto dx = itemSizeHint.width() + m_hspace;
    const auto dy = itemSizeHint.height() + m_vspace;

    const auto columns = qMin(m_items.size(), effectiveRect.size().width() / dx);
    const auto rows = (m_items.size() + columns - 1) / columns; /* Rounding the integer division up, e.g. 3 / 2 = 2 */


    const auto xpos = effectiveRect.x() + (effectiveRect.width() / 2) - ((dx * columns - m_hspace) / 2); /* Center the items horizontally */
    const auto ypos = effectiveRect.y();

    auto x = 0;
    auto y = 0;

    for(auto *item : m_items) {
        item->setGeometry(QRect(QPoint(x * dx + xpos, y * dy + ypos), itemSizeHint));

        if(++x == columns) {
            x = 0;
            ++y;

        }
    }
    return QSize(dx + left + right, dy * rows + top + bottom);
}

/************************************************
 * 					INSTRUMENT_BUTTON
 * **********************************************/

InstrumentButton::InstrumentButton(QString addr, TYPE_INSTRUMENT type, QWidget *parent):
    QPushButton(parent),
    m_statusText(new QLabel(this))
{
    address = addr;
    m_statusText->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

    QString name;
    switch(type){
    case TYPE_INSTR_COMPRESSION: {
        name = "Компрессионный прибор";
        break;
    }
    }


    auto *titleText = new QLabel(QString("INSTR:%1\t").arg(addr) + name, this);
    titleText->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    auto *image = new QLabel(this);
    image->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    //image->setPixmap(QPixmap(":/icons/device_medium.png"));
    QString iconPath = ":/icons/device_kl0_128.png";

    image->setPixmap(iconPath);
    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(titleText);
    layout->addWidget(image);
    layout->addWidget(m_statusText);

//    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//    titleText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//    image->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    m_statusText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

//    connect(m_instrument, &Instrument::stateChanged, this, &InstrumentButton::setStatus);
//    setStatus(Instrument::Ready);
//    image->resize(128, 128);
//    titleText->resize(128, 30);
//    m_statusText->resize(128, 30);
    layout->setSizeConstraint(QLayout::SetMinimumSize);
//    this->resize(150, 128);


}

void InstrumentButton::setStatus(int status)
{


//    if(status == Instrument::Ready) {
//        m_statusText->setText(tr("Готово"));
//        m_errorCount = 0;
//    } else if(status == Instrument::Experiment) {
//        m_statusText->setText(tr("Идет испытание"));
//        m_errorCount = 0;
//    } else if(status == Instrument::Error) {
//        if (m_errorCount >= SettingsManager::instance()->timeoutCount()) {
//            m_statusText->setText(tr("Ошибка"));
//            m_errorCount = 0;
//        }
//        else m_errorCount++;
//    } else {}


}

QString InstrumentButton::getAddress()
{
    return address;
}

//QString InstrumentButton::getIconName(int type)
//{


//    QString iconPath;

//    switch(type){
//    case Instrument::LIGA_KL0_1T:{}
//    case Instrument::LIGA_KL0_5T:{}
//    case Instrument::LIGA_KL0_25T:{}
//    case Instrument::LIGA_KL0S_1T:
//        iconPath = ":/icons/device_kl0_128.png";
//        break;
//    case Instrument::LIGA_KL1_1Vol_1T:
//    case Instrument::LIGA_KL1_2Vol_1T:
//    case Instrument::LIGA_KL1_1Vol_5T:
//    case Instrument::LIGA_KL1_2Vol_5T:
//    case Instrument::LIGA_KL1_1Vol_25T:
//    case Instrument::LIGA_KL1S_2Vol_1T:
//    case Instrument::LIGA_KL0S_2Load_1T:
//        iconPath = ":/icons/device_medium.png";
//        break;
//    }



//    return iconPath;
//}

void MainWindow::on_serverCmbBox_activated(const QString &arg1)
{
    if (!arg1.isEmpty())
        ui->mainLayout->addWidget( new ServerWindow(ui->serverCmbBox->currentText(), this));
}
