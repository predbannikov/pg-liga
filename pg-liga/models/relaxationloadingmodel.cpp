#include "relaxationloadingmodel.h"

#include <QPair>
#include <QDebug>

RelaxationLoadingModel::RelaxationLoadingModel(QObject *parent):
    AbstractSteppedModel(parent)
{
    
    
}

int RelaxationLoadingModel::rowCount(const QModelIndex &parent) const
{
    

    if(parent.isValid()) {
        
        return 0;
    } else {
        
        return m_steps.size();
    }

    
}

int RelaxationLoadingModel::columnCount(const QModelIndex &parent) const
{
    

    if(parent.isValid()) {
        
        return 0;
    } else {
        
        return ColumnCount;
    }

    
}

QVariant RelaxationLoadingModel::data(const QModelIndex &index, int role) const
{
    

    const auto row = index.row();
    const auto col = index.column();
    const auto currentStep = step(row);

    if((role == Qt::EditRole) || (role == Qt::DisplayRole)) {
        switch(col) {
        case StepParam:
            if(currentStep.type == Step::Deformation) {
                
                return QVariant::fromValue(QPair<int, double>(currentStep.type, currentStep.deform.millimetres()));
            } else if (currentStep.type == Step::Pressure) {
                
                return QVariant::fromValue(QPair<int, double>(currentStep.type, currentStep.relativePressure));
            } else {
                
                return QVariant();
            }
        case CriterionType:
            
            return currentStep.criterion;
        case Parameter:
            if(currentStep.criterion == Step::Stabilisation) {
                
                return QVariant::fromValue(currentStep.speedCritetion.kiloPascals());
            } else {
                
                return QVariant();
            }
        case Time:
            switch(currentStep.criterion){
            case Step::Manual: {
                
                return QVariant();
            }
            case Step::Duration: {
                
                return currentStep.durationTime.toString();
            }
            case Step::Stabilisation: {
                
                return currentStep.stabilisationTime.toString();
            }
            }
        default:
            
            return QVariant();
        }

    } else {
        
        return QVariant();
    }

    
}

bool RelaxationLoadingModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    

    //SanityCheck

    if(role == Qt::EditRole || role == Qt::DisplayRole) {
        const auto row = index.row();
        const auto col = index.column();
        auto currentStep = step(row);

        //QPair<int, double> stepParam;
        QPair<int, double> stepParam;

        switch(col) {
        case StepParam:
            stepParam = value.value<QPair<int,double>>();

            if(currentStep.type == Step::Deformation){
                currentStep.type = stepParam.first;
                currentStep.deform = Measurements::Length::fromMillimetres(stepParam.second);
            }
            else if (currentStep.type == Step::Pressure){
                currentStep.type = stepParam.first;
                currentStep.relativePressure = stepParam.second;
            }
            else {
                
                return false;
            }

            break;

        case CriterionType:
            currentStep.criterion = value.toInt();
            break;

        case Parameter:
            if (currentStep.criterion == Step::Stabilisation) currentStep.speedCritetion = Measurements::Pressure::fromKiloPascals(value.toDouble());
            break;

        case Time:
            switch(currentStep.criterion) {
            case Step::Duration:
                currentStep.durationTime = value.value<Measurements::TimeInterval>();
                break;

            case Step::Stabilisation:
                currentStep.stabilisationTime = value.value<Measurements::TimeInterval>();
                break;

            case Step::Manual:
            default:
                break;
            }
            break;
        }

        setStep(index, currentStep);
        
        return true;

    } else {
        
        return false;
    }

    
}

QVariant RelaxationLoadingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    

    const QStringList headers = {
        tr("Целевое значение"),
        tr("Критерий перехода\nк следующей ступени"),
        tr("Параметр стабилизации\nКПа"),
        tr("Время")
    };

    if((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
        
        return headers.at(section);
    } else {
        
        return QAbstractTableModel::headerData(section, orientation, role);
    }

//  
}

Qt::ItemFlags RelaxationLoadingModel::flags(const QModelIndex &index) const
{
    

    Q_UNUSED(index)

    

    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

int RelaxationLoadingModel::insertStep(const QModelIndex &index)
{
    

    const auto row = index.row() + 1;

    beginInsertRows(QModelIndex(), row, row);

    Step newStep;

    if(index.isValid()) {
        newStep.deform = m_steps.at(index.row()).deform;
    }

    m_steps.insert(row, newStep);
    endInsertRows();

    

    return row;
}

int RelaxationLoadingModel::removeStep(const QModelIndex &index)
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
    if (row == m_steps.count())
    {
        
        return row - 1;                                                        // return row - 1   : чтобы оставить фокус на таблице, когда удаляем последнюю строку
    }
    else
    {
        
        return row;                                                            // row              : чтобы оставить фокус на таблице, когда удаляем внутри строк
    }

    
}

int RelaxationLoadingModel::moveStep(const QModelIndex &index, int moveAmount)
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

int RelaxationLoadingModel::duplicateStep(const QModelIndex &index)
{
    if (m_steps.count() <= 0)
        return -1;
    const auto row = index.row() + 1;
    beginInsertRows(QModelIndex(), row, row);
    Step step = m_steps.at(index.row());
    m_steps.insert(row, step);
    endInsertRows();
    return row;
}

/*int RelaxationLoadingModel::insertInitialStep(Measurements::Pressure speed, const QModelIndex &index)
{
    

    const auto row = index.row() + 1;

    if((row >= 0) && (row < m_steps.count())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_steps.removeAt(row);
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), row, row);

    Step newStep;

    newStep.pressure = speed;
    m_steps.insert(row, newStep);

    endInsertRows();

    

    return row;
}*/

void RelaxationLoadingModel::setStep(const QModelIndex &index, const RelaxationLoadingModel::Step &step)
{
    

    m_steps.replace(index.row(), step);
    emit dataChanged(QModelIndex(), QModelIndex());

    
}
