#include "steppedmodeleditor.h"
#include "ui_steppedmodeleditor.h"

#include "appstrings.h"
#include "viewdelegate.h"

#include "steppedloadingmodel.h"
#include "consolidationaltmodel.h"
#include "steppedpressurisemodel.h"
#include "advancedsaturationmodel.h"
#include "advancedkinematicloadingmodel.h"
#include "consolidationmodel.h"
#include "relaxationloadingmodel.h"
#include "deformmodel.h"

SteppedModelEditor::SteppedModelEditor(QWidget *parent):
    QWidget(parent),
    ui(new Ui::SteppedModelEditor)
{
    ui->setupUi(this);

    auto height = fontMetrics().height();
    ui->stepView->verticalHeader()->setMinimumSectionSize(height + 8);
    ui->stepView->verticalHeader()->setDefaultSectionSize(height + 8);
    ui->stepView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

SteppedModelEditor::~SteppedModelEditor()
{
    delete ui;
}

void SteppedModelEditor::setModel(AdvancedSaturationModel *model)
{
    auto *cellPressureDelegate = new SpinBoxDelegate(Strings::kPa, model->getOverPressureCellVolumeter().kiloPascals(), this);
    cellPressureDelegate->setSingleStep(5);
    cellPressureDelegate->setDecimals(0);

    auto *porePressureDelegate = new SpinBoxDelegateAuto(Strings::kPa, model->getOverPressurePoreVolumeter().kiloPascals(), this);
    porePressureDelegate->setSingleStep(5);
    porePressureDelegate->setDecimals(0);

    auto *endCriterionDelegate = new ComboBoxDelegate({tr("вручную"), tr("время"), tr("стабилизация") }, this);
    //auto *parameterDelegate = new WeightedDelegate({Strings::kPa, Strings::percent}, {0, 2}, this);
    auto *parameterDelegate = new WeightedDelegate({Strings::kPa, Strings::percent}, {0, 2}, 100, this);
    auto *timeIntervalDelegate = new TimeIntervalDelegate(this);

    setupDelegates({
        cellPressureDelegate,
        porePressureDelegate,
        endCriterionDelegate,
        parameterDelegate,
        timeIntervalDelegate
    });

    connect(model, &AdvancedSaturationModel::dataChanged, this, &SteppedModelEditor::dataChanged);
    setupModel(model);
}

void SteppedModelEditor::setModel(ConsolidationModel *model)
{
    auto *pressureDelegate = new SpinBoxDelegate(Strings::kPa, 9999, this);
    pressureDelegate->setSingleStep(5);
    pressureDelegate->setDecimals(0);

    auto *endCriterionDelegate = new ComboBoxDelegate({tr("вручную"), tr("стабилизация"), tr("время") }, this);

    auto parameterDelegate = new SpinBoxDelegate(Strings::percent, 99.99, this);

    parameterDelegate->setSingleStep(0.05);
    parameterDelegate->setDecimals(2);

    auto *timeLongIntervalDelegate = new TimeLongIntervalDelegate(this);

    setupDelegates({
        pressureDelegate,
        endCriterionDelegate,
        parameterDelegate,
        timeLongIntervalDelegate
    });

    connect(model, &ConsolidationModel::dataChanged, this, &SteppedModelEditor::dataChanged);
    setupModel(model);
}


void SteppedModelEditor::setModel(ConsolidationAltModel *model)
{
    auto *pressureDelegate = new SpinBoxDelegate(Strings::kPa, 9999, this);
    pressureDelegate->setSingleStep(5);
    pressureDelegate->setDecimals(0);

    auto *endCriterionDelegate = new ComboBoxDelegate({tr("вручную"), tr("время"), tr("стабилизация") }, this);
    //auto *parameterDelegate = new WeightedDelegate({Strings::cm3, Strings::percent}, {3, 2}, this);
    auto *parameterDelegate = new WeightedDelegate({Strings::cm3, Strings::percent}, {3, 2}, 100, this);
    auto *timeIntervalDelegate = new TimeIntervalDelegate(this);

    setupDelegates({
        pressureDelegate,
        endCriterionDelegate,
        parameterDelegate,
        timeIntervalDelegate
    });

    connect(model, &ConsolidationAltModel::dataChanged, this, &SteppedModelEditor::dataChanged);
    setupModel(model);
}

void SteppedModelEditor::setModel(SteppedLoadingModel *model)
{
    auto *pressureDelegate = new WeightedDelegate({Strings::kPa, Strings::N}, {3, 2}, 999999, this);    //1t - 9999  5t - 49999 30t - 49999
//    pressureDelegate->setSingleStep(5);
//    pressureDelegate->setDecimals(1);

    auto *endCriterionDelegate = new ComboBoxDelegate({tr("вручную"), tr("время"), tr("стабилизация") }, this);
    //auto *parameterDelegate = new WeightedDelegate({Strings::mm, Strings::percent}, {3, 2}, this);
    auto *parameterDelegate = new WeightedDelegate({Strings::mm, Strings::percent}, {3, 2}, 100, this);
//    auto *timeIntervalDelegate = new TimeIntervalDelegate(this);
    auto *timeIntervalDelegate = new TimeLongIntervalDelegate(this);

    setupDelegates({
        pressureDelegate,
        endCriterionDelegate,
        parameterDelegate,
        timeIntervalDelegate
    });

    connect(model, &SteppedLoadingModel::dataChanged, this, &SteppedModelEditor::dataChanged);
    setupModel(model);
}

void SteppedModelEditor::setModel(SteppedPressuriseModel *model)
{

    auto *pressureDelegate = new SpinBoxDelegate(Strings::kPa, model->getOverPressureCellVolumeter().kiloPascals(), this);
    pressureDelegate->setSingleStep(5);
    pressureDelegate->setDecimals(0);

    auto *endCriterionDelegate = new ComboBoxDelegate({tr("вручную"), tr("время"), tr("стабилизация") }, this);
    //auto *parameterDelegate = new WeightedDelegate({Strings::kPa, Strings::percent}, {0, 2}, this);
    auto *parameterDelegate = new WeightedDelegate({Strings::kPa, Strings::percent}, {0, 2}, 100, this);
    auto *timeIntervalDelegate = new TimeIntervalDelegate(this);

    setupDelegates({
        pressureDelegate,
        endCriterionDelegate,
        parameterDelegate,
        timeIntervalDelegate
    });

    setupModel(model);
    connect(model, &SteppedPressuriseModel::dataChanged, this, &SteppedModelEditor::dataChanged);
}

void SteppedModelEditor::setModel(AdvancedKinematicLoadingModel *model)
{
    auto *speedDelegate = new SpinBoxDelegate(Strings::mmMin, 200, this);
    speedDelegate->setSingleStep(0.1);
    speedDelegate->setDecimals(3);

    auto *typeDelegate = new ComboBoxDelegate({tr("осевое напряжение"), tr("осевая деформация") }, this);
    auto *criterionDelegate = new SpinBoxDelegateMulti({Strings::kPa, Strings::percent}, {0, 2}, this);

    setupDelegates({
        speedDelegate,
        typeDelegate,
        criterionDelegate
    });

    connect(model, &AdvancedKinematicLoadingModel::dataChanged, this, &SteppedModelEditor::dataChanged);
    setupModel(model);
}

void SteppedModelEditor::setModel(RelaxationLoadingModel *model)
{
    //auto *stepDelegate = new WeightedDelegate({Strings::mm, Strings::kPa}, {3, 2}, 10000, this);
    auto *stepDelegate = new WeightedDelegate({Strings::mm, Strings::kPa + " " + Strings::percent}, {3, 2}, 50, this);
    auto *endCriterionDelegate = new ComboBoxDelegate({tr("вручную"), tr("время"), tr("стабилизация") }, this);
    auto *criterionDelegate = new SpinBoxDelegate(Strings::kPa, 1000, this);
    auto *timeIntervalDelegate = new TimeIntervalDelegate(this);

    criterionDelegate->setDecimals(2);

    setupDelegates({
        stepDelegate,
        endCriterionDelegate,
        criterionDelegate,
        timeIntervalDelegate
    });

    connect(model, &RelaxationLoadingModel::dataChanged, this, &SteppedModelEditor::dataChanged);
    setupModel(model);
}

void SteppedModelEditor::setModel(DeformModel *model)
{
    auto *endCriterionDelegate = new ComboBoxDelegate({tr("вручную"), tr("время"), tr("стабилизация") }, this);
    auto *parameterValueDelegate = new WeightedDelegate({Strings::mm, Strings::percent, Strings::kPa}, {3, 2, 0}, 100000, this);
    auto *timeIntervalDelegate = new TimeLongIntervalDelegate(this);
    ui->stepView->setItemDelegateForColumn(DeformModel::ColumnCritery, endCriterionDelegate);
    ui->stepView->setItemDelegateForColumn(DeformModel::ColumnParamStabilization, parameterValueDelegate);
    ui->stepView->setItemDelegateForColumn(DeformModel::ColumnTime, timeIntervalDelegate);
    auto height = fontMetrics().height();
    ui->stepView->verticalHeader()->setMinimumSectionSize(height + 8);
    ui->stepView->verticalHeader()->setDefaultSectionSize(height + 8);
    ui->stepView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(model, &DeformModel::dataChanged, this, &SteppedModelEditor::dataChanged);
    setupModel(model);
}

void SteppedModelEditor::addStep()
{
    ui->btnLeftStepAdd->clicked();
}

void SteppedModelEditor::clearStep(int idx)
{
    const auto *model = ui->stepView->model();
    ui->stepView->setCurrentIndex(model->index(idx, 0));
    ui->btnMiddleStepRemove->clicked();
}

QJsonObject SteppedModelEditor::serializModel()
{
    QAbstractTableModel *tableModel = qobject_cast<QAbstractTableModel *>(ui->stepView->model());
    if (qobject_cast<SteppedPressuriseModel *>(tableModel) != nullptr) {
        SteppedPressuriseModel *model = qobject_cast<SteppedPressuriseModel *>(tableModel);
        qDebug() << Q_FUNC_INFO << "SteppedPressuriseModel" << model->serializModel();

    }
    else if (qobject_cast<SteppedLoadingModel *>(tableModel) != nullptr) {
        SteppedLoadingModel *model = qobject_cast<SteppedLoadingModel *>(tableModel);
        qDebug() << Q_FUNC_INFO << "SteppedLoadingModel";

    }
    return QJsonObject();
}

void SteppedModelEditor::setStepHighlighted(int idx)
{
    const auto *model = ui->stepView->model();
    ui->stepView->setCurrentIndex(model->index(idx, 0));
}

void SteppedModelEditor::setupModel(AbstractSteppedModel *model)
{
    ui->stepView->setModel(model);

    connect(ui->btnLeftStepAdd, &QPushButton::clicked, [=]() {
        setStepHighlighted(model->insertStep(ui->stepView->currentIndex()));
    });

    connect(ui->btnMiddleStepRemove, &QPushButton::clicked, [=]() {
        setStepHighlighted(model->removeStep(ui->stepView->currentIndex()));
    });

    connect(ui->btnMiddleStepUp, &QPushButton::clicked, [=]() {
        setStepHighlighted(model->moveStep(ui->stepView->currentIndex(), -1));
    });

    connect(ui->btnRightStepDown, &QPushButton::clicked, [=]() {
        setStepHighlighted(model->moveStep(ui->stepView->currentIndex(), 1));
    });

    connect(ui->btnDuplicateStep, &QPushButton::clicked, [=]() {
        setStepHighlighted(model->duplicateStep(ui->stepView->currentIndex()));
    });

}

void SteppedModelEditor::setupDelegates(const QVector<QStyledItemDelegate*> &delegates)
{
    for(auto i = 0; i < delegates.size(); ++i) {
        ui->stepView->setItemDelegateForColumn(i, delegates.at(i));
    }
}
