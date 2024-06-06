#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include "Settings/settingsmanager.h"
#include "Settings/settingsdialog.h"
#include "clientwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class InstrumentButton;
class InstrumentGrid;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onTabCloseRequested(int idx);
    void onInstrumentSelected(QString name);

    /* Auto slots */
    void on_actExit_triggered();
    void on_actSettings_triggered();
    void addInstruments();

    void exitWindow();

private:
    void setFirstTab(QWidget *widget, const QString &title);

    void addServersWindow(const QMap<QString, QVariant> map);

    InstrumentGrid *grid = nullptr;
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
    void instrumentSelected(QString name);

public slots:
    void addInstrument(QString name);
    void removeInstrument(QString name);

private slots:
    void onInstrumentButtonClicked();

private:
    InstrumentButton *findButton(QString name);
    FlowLayout *m_layout;
};

class InstrumentButton : public QPushButton
{
    Q_OBJECT

public:
    InstrumentButton(QString name, QWidget *parent = nullptr);

public slots:
    QString getSN();

private:
//    QString getIconName(int type);

    QString m_SN;
    QLabel *m_statusText;
};

#endif // MAINWINDOW_H
