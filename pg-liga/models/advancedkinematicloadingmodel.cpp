#include "advancedkinematicloadingmodel.h"

#include <QPair>
#include <QDebug>

AdvancedKinematicLoadingModel::AdvancedKinematicLoadingModel(QObject *parent):
    AbstractSteppedModel(parent)
{
    
    
}

int AdvancedKinematicLoadingModel::rowCount(const QModelIndex &parent) const
{
    

    if(parent.isValid()) {
        
        return 0;
    } else {
        
        return m_steps.size();
    }

    
}

int AdvancedKinematicLoadingModel::columnCount(const QModelIndex &parent) const
{
    

    if(parent.isValid()) {
        
        return 0;
    } else {
        
        return ColumnCount;
    }

    
}

QVariant AdvancedKinematicLoadingModel::data(const QModelIndex &index, int role) const
{
    

    const auto row = index.row();
    const auto col = index.column();
    const auto currentStep = step(row);

    if((role == Qt::EditRole) || (role == Qt::DisplayRole)) {
        switch(col) {
        case LoadingSpeed:
            
            return currentStep.loadingSpeed.millimetresPerMinute();
        case CriterionType:
            
            return currentStep.type;
        case Parameter:
            if(currentStep.type == Step::Pressure) {
                
                return QVariant::fromValue(QPair<int, double>(currentStep.type, currentStep.pressureCriterion.kiloPascals()));
            } else if(currentStep.type == Step::Deformation) {
                
                return QVariant::fromValue(QPair<int, double>(currentStep.type, currentStep.deformCriterion * 100.0));
            } else {
                
                return QVariant();
            }

        default:
            
            return QVariant();
        }

    } else {
        
        return QVariant();
    }

    
}

bool AdvancedKinematicLoadingModel::setData(const QModelIndex &index, const QVariant &value, int role)  //manual set
{
    

    const auto overloadingspeed = Measurements::Speed::fromMillimetresPerMinute(20.0);

    const auto maxPressure = Measurements::Pressure::fromForce(Measurements::Force::fromKiloNewtons(220.0),
                                                               Measurements::Area::circle(Measurements::Length::fromMillimetres(20)));  //TODO make it read actual exp parameters

    if(role == Qt::EditRole || role == Qt::DisplayRole) {
        const auto row = index.row();
        const auto col = index.column();
        auto currentStep = step(row);

        QPair<int, double> param;

        switch(col) {
        case LoadingSpeed:
            // Клиент задаёт значения любое по факту на 1 и 2 ступени скорость задаётся в операции статично 0.02
//            if (row == 0 || row == 1) {
//                currentStep.loadingSpeed = Measurements::Speed::fromMillimetresPerMinute(0.03);
//            } else if (Measurements::Speed::fromMillimetresPerMinute(value.toDouble()) >= overloadingspeed
//                    || Measurements::Speed::fromMillimetresPerMinute(value.toDouble()) == Measurements::Speed::fromMillimetresPerMinute(0.0))
//                 currentStep.loadingSpeed = Measurements::Speed::fromMillimetresPerMinute(0.2);
//            else
                currentStep.loadingSpeed = Measurements::Speed::fromMillimetresPerMinute(value.toDouble());
            break;

        case CriterionType:
            currentStep.type = value.toInt();
            break;

        case Parameter:
            param = value.value<QPair<int, double>>();

            if(param.first == Step::Pressure) {
                if (Measurements::Pressure::fromKiloPascals(param.second) >= maxPressure)
                     currentStep.pressureCriterion = Measurements::Pressure::fromKiloPascals(100);
                else currentStep.pressureCriterion = Measurements::Pressure::fromKiloPascals(param.second);
            } else if(param.first == Step::Deformation) {
                if((param.second / 100.0) <= 0.0 || (param.second / 100.0) >= 1.0)
                     currentStep.deformCriterion = 0.15;
                else currentStep.deformCriterion = param.second / 100.0;
            } else {}

            break;

        default:
            break;
        }

        setStep(index, currentStep);
        
        return true;

    } else {
        return false;
    }

    
}

QVariant AdvancedKinematicLoadingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    

    const QStringList headers = {
        tr("Скорость нагружения"),
        tr("Критерий завершения"),
        tr("Целевое значение")
    };

    if((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
        
        return headers.at(section);
    } else {
        
        return QAbstractTableModel::headerData(section, orientation, role);
    }

//  
}

Qt::ItemFlags AdvancedKinematicLoadingModel::flags(const QModelIndex &index) const
{
    

    Q_UNUSED(index)

    

    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

int AdvancedKinematicLoadingModel::insertStep(const QModelIndex &index)
{
    

    const auto row = index.row() + 1;

    beginInsertRows(QModelIndex(), row, row);

    Step newStep;

    if(index.isValid()) {
        newStep.loadingSpeed = m_steps.at(index.row()).loadingSpeed;
    }

    m_steps.insert(row, newStep);
    endInsertRows();

    

    return row;
}

int AdvancedKinematicLoadingModel::removeStep(const QModelIndex &index)
{
    

    const auto row = index.row();

    if((row >= 0) && (row < m_steps.count())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_steps.removeAt(row);
        endRemoveRows();
    }
/*
    if (row == m_steps.count()) return row - 1;                                 // return row - 1   : чтобы оставить фокус на таблице, когда удаляем последнюю строку
    else return row;                                                            // row              : чтобы оставить фокус на таблице, когда удаляем внутри строк
*/
    if (row == m_steps.count())                                 // return row - 1   : чтобы оставить фокус на таблице, когда удаляем последнюю строку
    {
        
        return row - 1;
    }
    else                                                        // row              : чтобы оставить фокус на таблице, когда удаляем внутри строк
    {
        
        return row;
    }

    
}

int AdvancedKinematicLoadingModel::moveStep(const QModelIndex &index, int moveAmount)
{
    

    const auto row = index.row();
    if(row < 0 || row >= m_steps.count()) {
        
        return -1;
    }

    auto targetRow = row + moveAmount;

    if(targetRow < 0) {
        targetRow = 0;
    }

    if(targetRow >= m_steps.count()) {
        targetRow = m_steps.count() - 1;
    }

    if(row == targetRow) {
        
        return -1;
    }

    const auto tmp = step(targetRow);
    m_steps.replace(targetRow, m_steps.at(row));
    m_steps.replace(row, tmp);

    emit dataChanged(QModelIndex(), QModelIndex());

    

    return targetRow;
}

int AdvancedKinematicLoadingModel::duplicateStep(const QModelIndex &index)
{
    if (m_steps.count() <= 0)
        return -1;
    if (index.row() < 0)
        return -1;
    const auto row = index.row() + 1;
    beginInsertRows(QModelIndex(), row, row);
    Step step = m_steps.at(index.row());
    m_steps.insert(row, step);
    endInsertRows();
    return row;
}

/*int AdvancedKinematicLoadingModel::insertInitialStep(Measurements::Speed speed, const QModelIndex &index)
{
    

    const auto row = index.row() + 1;

    if((row >= 0) && (row < m_steps.count())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_steps.removeAt(row);
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), row, row);

    Step newStep;

    newStep.loadingSpeed = speed;
    m_steps.insert(row, newStep);

    endInsertRows();

    

    return row;
}*/

void AdvancedKinematicLoadingModel::setStep(const QModelIndex &index, const AdvancedKinematicLoadingModel::Step &step)
{
    

    m_steps.replace(index.row(), step);
    emit dataChanged(QModelIndex(), QModelIndex());

    
}
