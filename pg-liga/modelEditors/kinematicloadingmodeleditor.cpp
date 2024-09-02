#include <QDebug>

#include "kinematicloadingmodeleditor.h"
#include "ui_kinematicloadingmodeleditor.h"

#include "appstrings.h"
#include "viewdelegate.h"
#include "kinematicloadingmodel.h"
#include "kinematicpressurisedloadingmodel.h"

KinematicLoadingModelEditor::KinematicLoadingModelEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KinematicLoadingModelEditor)
{
    

    ui->setupUi(this);

    // TODO: resolve this with styles if possible
    auto height = fontMetrics().height();
    ui->kinematicView->verticalHeader()->setMinimumSectionSize(height + 8);
    ui->kinematicView->verticalHeader()->setDefaultSectionSize(height + 8);

    auto *parameterDelegate = new WeightedDelegate({Strings::mm, Strings::percent}, {3, 2}, 100, this);

    ui->kinematicView->setItemDelegateForColumn(KinematicLoadingModel::TargetDeform, parameterDelegate);

    ui->kinematicView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->kinematicView->verticalHeader()->setVisible(false);

    
}

KinematicLoadingModelEditor::~KinematicLoadingModelEditor()
{
    

    delete ui;

    
}

void KinematicLoadingModelEditor::setModel(KinematicLoadingModel *model)
{
    

    m_model = model;
    ui->kinematicView->setModel(model);

    auto *speedDelegate = new SpinBoxDelegate(Strings::mmMin, 200, this);
    speedDelegate->setSingleStep(0.001);
    speedDelegate->setDecimals(4);

    ui->kinematicView->setItemDelegateForColumn(KinematicLoadingModel::PressureSpeed, speedDelegate);

    
}

void KinematicLoadingModelEditor::setModel(KinematicPressurisedLoadingModel *model)
{
    

    m_model = model;
    ui->kinematicView->setModel(model);

    auto *speedDelegate = new SpinBoxDelegate(Strings::kPaSec, 5001, this);
    speedDelegate->setSingleStep(100);
    speedDelegate->setDecimals(0);

    ui->kinematicView->setItemDelegateForColumn(KinematicLoadingModel::PressureSpeed, speedDelegate);

    
}

void KinematicLoadingModelEditor::setModel(QAbstractTableModel *model)
{
    

    m_model = model;
    ui->kinematicView->setModel(model);

    
}
