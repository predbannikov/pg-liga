#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include "Settings/settingsmanager.h"
#include "Settings/settingsdialog.h"
#include "serverwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum TYPE_INSTRUMENT {
    TYPE_INSTR_COMPRESSION
};

class InstrumentButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

//    ClientExperiment *clientExperiment;
//    ServerWindow *serverWindow = nullptr;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void scanInstruments();
    void onTabCloseRequested(int idx);
    void onScanFinished();
    void onInstrumentSelected(TYPE_INSTRUMENT type);

    /* Auto slots */
    void on_actExit_triggered();
//    void on_actScan_triggered();
    void on_actSettings_triggered();


    void exitWindow();



    void on_serverCmbBox_activated(const QString &arg1);

private:
    void setFirstTab(QWidget *widget, const QString &title);


    Ui::MainWindow *ui;
};

class FlowLayout : public QLayout
{
    Q_OBJECT

public:
    FlowLayout(QWidget *parent = nullptr);
    ~FlowLayout();

    void addItem(QLayoutItem *item) override;

    QSize sizeHint() const override;
    QSize minimumSize() const override;

    void setGeometry(const QRect &r) override;

    QLayoutItem *itemAt(int index) const override;
    QLayoutItem *takeAt(int index) override;

    int count() const override;

private:
    QSize buildLayout(const QRect &r) const;

    QList<QLayoutItem*> m_items;
    int m_hspace = 6;
    int m_vspace = 6;
    QSize m_minSize;
};

class InstrumentGrid : public QScrollArea
{
    Q_OBJECT

public:
    InstrumentGrid(QWidget *parent = nullptr);

signals:
    void instrumentSelected(TYPE_INSTRUMENT type);

public slots:
    void addInstrument(TYPE_INSTRUMENT type);
    void removeInstrument(TYPE_INSTRUMENT type);

private slots:
    void onInstrumentButtonClicked();

private:
    InstrumentButton *findButton(TYPE_INSTRUMENT type);
    FlowLayout *m_layout;
};

class InstrumentButton : public QPushButton
{
    Q_OBJECT

public:
    InstrumentButton(TYPE_INSTRUMENT type, QWidget *parent = nullptr);
    TYPE_INSTRUMENT instrument() const { return type_instr; }

public slots:
    void setStatus(int status);

private:
//    QString getIconName(int type);

    TYPE_INSTRUMENT type_instr;
    QLabel *m_statusText;
};

#endif // MAINWINDOW_H
