#ifndef OPERATIONACTIONS_H
#define OPERATIONACTIONS_H

#include <QWidget>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QIcon>
#include <QStyle>

#include "steppedmodeleditor.h"
#include "steppedloadingmodel.h"
#include "steppedpressurisemodel.h"

static const auto defaultText = QObject::tr("Действие не установлено");

namespace Ui {
class OperationActions;
}

class OperationActions : public QWidget
{
    Q_OBJECT

public:
    explicit OperationActions(int numberOperation, QWidget *parent = nullptr);
    ~OperationActions();
    int numberOperation() { return m_numOperation; }
    void setNumberOperation(int numberOperation);

    void contextMenuEvent(QContextMenuEvent *event);

public slots:
    void createSteppedPressureWgt();
    void createSteppedLoadingWgt();

private slots:
    void on_btnMenuActions_clicked();

signals:
    void addOperationActions();
    void deleteOperationActions();
    void moveOperationUpActions();
    void moveOperationDownActions();
private:
    void createMenu();
    void deleteWidget();
    void updateTextMenu(QString text = "");

    Ui::OperationActions *ui;

    QAction *steppedPressureAct = nullptr;
    QAction *steppedLoadingAct = nullptr;
    QAction *deleteAct = nullptr;
    QAction *addOperationAct = nullptr;
    QAction *moveUpOperationAct = nullptr;
    QAction *moveDownOperationAct = nullptr;


    QString currentTextMenu;

    int m_numOperation = -1;
};

#endif // OPERATIONACTIONS_H
