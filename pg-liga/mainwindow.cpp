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
        qDebug() << Q_FUNC_INFO << tr("Не удалось загрузить настройки");
    }
//    this->resize(1280, 768);

    qDebug() << tr("Версия: <b>%1.%2</b> Ревизия: <b>%3</b>").arg(VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);

    this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));



    tabWidget = new CustomTabWidget(this);
    tabWidget->setMovable(true);
    tabWidget->setTabsClosable(true);
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);
//    QVBoxLayout *layout = new QVBoxLayout(this);
//    this->setLayout(layout);

    this->setCentralWidget(tabWidget);

    grid = new InstrumentGrid;
    tabWidget->insertTab(0, grid, "Список");

    QTimer::singleShot(100, this, &MainWindow::addInstruments);

    connect(&timerFFlush, &QTimer::timeout, [](){
        fflush(stderr);
    });
    timerFFlush.start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onInstrumentSelected(QString name)
{
    QWidget *tab = nullptr;
    for (int i = 0; i < tabWidget->count(); i++) {
        if (tabWidget->tabText(i) == name) {
            tab = tabWidget->widget(i);
        }
    }

    if (tab == nullptr) {
        tab = new ClientWindow(name);
        tabWidget->addTab(tab, name);
    }
    tabWidget->setCurrentWidget(tab);
}

void MainWindow::on_actExit_triggered()
{
    qDebug() << Q_FUNC_INFO;
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

void MainWindow::addInstruments()
{
    auto map = SettingsManager::instance()->getServers();
    auto list = map.keys();
    for (const auto &device: qAsConst(list)) {
        grid->addInstrument(device);
        connect(grid, &InstrumentGrid::instrumentSelected, this, &MainWindow::onInstrumentSelected);
    }
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
    if(tabWidget->tabText(idx) != "Список") {
        tabWidget->widget(idx)->deleteLater();
        tabWidget->removeTab(idx);
//        auto *instrument = m_instrumentsInUse.value(tabWidget->clo->widget(idx), nullptr);
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

void InstrumentGrid::addInstrument(QString name)
{
    auto *button = new InstrumentButton(name, this);
    connect(button, &QPushButton::clicked, this, &InstrumentGrid::onInstrumentButtonClicked);

    m_layout->addWidget(button);
}

void InstrumentGrid::removeInstrument(QString name)
{
    auto *btn = findButton(name);
    if(btn != nullptr) {
        m_layout->removeWidget(btn);
    }
}

void InstrumentGrid::onInstrumentButtonClicked()
{
    auto instrumentButton = qobject_cast<InstrumentButton*>(sender());
    emit instrumentSelected(instrumentButton->getSN());
}

InstrumentButton *InstrumentGrid::findButton(QString name)
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
//    if (r.x())
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
    if (columns == 0)
        return QSize();
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

InstrumentButton::InstrumentButton(QString name, QWidget *parent):
    QPushButton(parent),
    m_statusText(new QLabel(this))
{
    m_SN = name;
    m_statusText->setAlignment(Qt::AlignLeft | Qt::AlignBottom);




    auto *titleText = new QLabel(QString("%1\t").arg(name), this);
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



QString InstrumentButton::getSN()
{
    return m_SN;
}
