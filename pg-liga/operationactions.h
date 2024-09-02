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
#include "kinematicpressurisedloadingmodel.h"

/*
 *  Для добавления действия операции необходимо:
 *  1. Добавить QAction переменную                                      (example *steppedPressureAct = nullptr;)
 *  2. Создать слот для создания виджета и добавления в лайоут          (example     void createSteppedPressureWgt();)
 *  3. Создать реализацию по шаблону
 *          deleteWidget();
            SteppedModelEditor *wgtStepEdit = new SteppedModelEditor(this);
            StaticDeviatorModel *model = new StaticDeviatorModel(wgtStepEdit);
            wgtStepEdit->setModel(model);
            ui->widget->layout()->addWidget(wgtStepEdit);
            updateTextMenu(steppedLoadingAct->text());

 *
 */

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
    SteppedModelEditor *createSteppedModelEditor(QString text);

    Ui::OperationActions *ui;

    QList <QAction *> actions;
//    QAction *steppedPressureAct = nullptr;
//    QAction *steppedLoadingAct = nullptr;
//    QAction *steppdDeviatorAct = nullptr;
//    QAction *kinematicPressurisedLoadingAct = nullptr;

    QAction *deleteAct = nullptr;
    QAction *addOperationAct = nullptr;
    QAction *moveUpOperationAct = nullptr;
    QAction *moveDownOperationAct = nullptr;


    QString currentTextMenu;

    int m_numOperation = -1;
};

#endif // OPERATIONACTIONS_H
