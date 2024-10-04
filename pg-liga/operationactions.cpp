#include "operationactions.h"
#include "ui_operationactions.h"

#include <QJsonDocument>

OperationActions::OperationActions(int numberOperation, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OperationActions),
    m_numOperation(numberOperation)
{
    ui->setupUi(this);


    ui->btnMenuActions->setStyleSheet("background-color: #e6f0e6; border: 0px solid; ");
    currentTextMenu = defaultText;
    updateTextMenu();

    QVBoxLayout *lay = new QVBoxLayout;
    lay->setSpacing(0);
    lay->setMargin(0);
    ui->widget->setLayout(lay);

    ui->groupBox->layout()->setSpacing(0);
    ui->groupBox->layout()->setMargin(0);

    {
        {
            actions.append(new QAction("Установка всестороннего давления", this));
            connect(actions.last(), &QAction::triggered, this, [=](){ createSteppedModelEditor()->setModel(new SteppedPressuriseModel); });
        }
        {
            actions.append(new QAction("Установка вертикального давления", this));
            connect(actions.last(), &QAction::triggered, this, [=](){ createSteppedModelEditor()->setModel(new StaticCompressionModel); });
        }
        {
            actions.append(new QAction("Установка осевого напряжения", this));
            connect(actions.last(), &QAction::triggered, this, [=](){ createSteppedModelEditor()->setModel(new StaticDeviatorModel); });
        }
    }


    deleteAct = new QAction("Удалить операцию", this);
    connect(deleteAct, &QAction::triggered, this, &OperationActions::deleteOperationActions);

    addOperationAct = new QAction("Добавить операцию", this);
    connect(addOperationAct, &QAction::triggered, this, &OperationActions::addOperationActions);

    moveUpOperationAct = new QAction("Переместить операцию вверх", this);
    connect(moveUpOperationAct, &QAction::triggered, this, &OperationActions::moveOperationUpActions);

    moveDownOperationAct = new QAction("Переместить операцию вниз", this);
    connect(moveDownOperationAct, &QAction::triggered, this, &OperationActions::moveOperationDownActions);

}

OperationActions::~OperationActions()
{
    delete ui;
}

void OperationActions::setNumberOperation(int numberOperation)
{
    m_numOperation = numberOperation;
    updateTextMenu();
}

void OperationActions::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event)
    //    QMenu menu(this);
//    menu.addAction(setPressureAct);
//    menu.exec(event->globalPos());
    createMenu();
}

void OperationActions::on_btnMenuActions_clicked()
{
    createMenu();
}

void OperationActions::createMenu()
{
    QMenu menu(this);
    QMenu actionsSubMenu("Действия", &menu);
    actionsSubMenu.addActions(actions);
    menu.addAction(addOperationAct);
    menu.addMenu(&actionsSubMenu);
    menu.addAction(moveUpOperationAct);
    menu.addAction(moveDownOperationAct);
    menu.addAction(deleteAct);
    menu.exec(QCursor::pos());
}

void OperationActions::deleteWidget()
{
    int count = ui->widget->layout()->count();
    if (count >= 1)
        delete ui->widget->layout()->itemAt(0)->widget();
    currentTextMenu = defaultText;
    updateTextMenu();
}

QJsonObject OperationActions::serializOperation()
{
    QJsonObject jObj;
    if (!ui->widget->layout()->isEmpty())
    {
        QWidget *wgt = ui->widget->layout()->itemAt(0)->widget();
        if (qobject_cast<SteppedModelEditor *>(wgt) != nullptr) {
            SteppedModelEditor *steppedModelEditor = qobject_cast<SteppedModelEditor *>(wgt);
            jObj = steppedModelEditor->serializModel();
            jObj["name"] = steppedModelEditor->getTypeStep();
            if (jObj["name"] == "steppedPressurise")
                jObj["betta"] = "0.95";
        }
        else if (qobject_cast<KinematicLoadingModelEditor *>(wgt) != nullptr)
        {
            qDebug() << "KinematicLoadingModelEditor";
        }
        else {
            qDebug() << "##################";
        }
        return jObj;
    }
    return QJsonObject();
}

void OperationActions::updateTextMenu(QString text)
{
    if (text.isEmpty())
        ui->lblText->setText(QString("Этап № %1    %2").arg(m_numOperation+1).arg(currentTextMenu));
    else {
        currentTextMenu = text;
        ui->lblText->setText(QString("Эпап № %1    %2").arg(m_numOperation+1).arg(text));
    }
}

SteppedModelEditor *OperationActions::createSteppedModelEditor()
{
    deleteWidget();
    SteppedModelEditor *wgtStepEdit = new SteppedModelEditor(this);
    ui->widget->layout()->addWidget(wgtStepEdit);
    updateTextMenu(qobject_cast<QAction *>(sender())->text());
    connect(wgtStepEdit, &SteppedModelEditor::dataChanged, this, &OperationActions::dataChanged);
    return wgtStepEdit;
}

