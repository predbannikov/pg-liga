#include <QDebug>
#include "advancedsaturationmodel.h"

AdvancedSaturationModel::AdvancedSaturationModel(Measurements::Pressure overPressureCellVolumeter,
                                                 Measurements::Pressure overPressurePoreVolumeter, QObject *parent):
    AbstractSteppedModel(parent),
    m_overPressureCellVolumeter(overPressureCellVolumeter),
    m_overPressurePoreVolumeter(overPressurePoreVolumeter)
{
    
    
}

int AdvancedSaturationModel::rowCount(const QModelIndex &parent) const
{
    

    if(parent.isValid()) {
        
        return 0;
    } else {
        
        return m_steps.size();
    }

    
}

int AdvancedSaturationModel::columnCount(const QModelIndex &parent) const
{
    

    if(parent.isValid()) {
        
        return 0;
    } else {
        
        return ColumnCount;
    }

    
}

QVariant AdvancedSaturationModel::data(const QModelIndex &index, int role) const
{
    

    const auto row = index.row();
    const auto col = index.column();
    const auto currentStep = step(row);

    if((role == Qt::EditRole) || (role == Qt::DisplayRole)) {
        switch(col) {
        case CellPressure:
            
            return currentStep.cellPressure.kiloPascals();
        case PorePressure:
            
            return currentStep.diffPressure.kiloPascals();
        case EndCriterion:
            
            return currentStep.criterion;
        case StabilisationParameter:
            if(currentStep.criterion == Step::Stabilisation) {
                if(currentStep.stabilisationType == Step::Absolute) {
                    
                    return QVariant::fromValue(QPair<int, double>(currentStep.stabilisationType, currentStep.stabilisationParamAbsolute.kiloPascals()));
                } else if(currentStep.stabilisationType == Step::Relative) {
                    
                    return QVariant::fromValue(QPair<int, double>(currentStep.stabilisationType, currentStep.stabilisationParamRelative * 100.0));
                } else {
                    
                    return QVariant();
                }

            } else {
                
                return QVariant();
            }

        case Time:
            switch(currentStep.criterion) {
            case Step::Duration:
                
                return QVariant::fromValue(currentStep.durationTime);
            case Step::Stabilisation:
                
                return QVariant::fromValue(currentStep.stabilisationTime);
            case Step::Manual:
            default:
                
                return QVariant();
            }

        default:
            
            return QVariant();
        }

    } else {
        
        return QVariant();
    }

    
}

bool AdvancedSaturationModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::EditRole || role == Qt::DisplayRole) {
        const auto row = index.row();
        const auto col = index.column();
        auto currentStep = step(row);

        QPair<int, double> param;

        switch(col) {
        case CellPressure:
            if (Measurements::Pressure::fromKiloPascals(value.toDouble()) <= Measurements::Pressure::fromPascals(0))
                currentStep.cellPressure = Measurements::Pressure::fromKiloPascals(50);
            else if (Measurements::Pressure::fromKiloPascals(value.toDouble()) >= m_overPressurePoreVolumeter)
                currentStep.cellPressure = Measurements::Pressure::fromKiloPascals(m_overPressurePoreVolumeter.kiloPascals() - Measurements::Pressure::fromKiloPascals(50).kiloPascals());
            else
                currentStep.cellPressure = Measurements::Pressure::fromKiloPascals(value.toDouble());
            break;

        case PorePressure:
            currentStep.diffPressure = Measurements::Pressure::fromKiloPascals(value.toDouble());
            break;

        case EndCriterion:
            currentStep.criterion = value.toInt();
            break;

        case StabilisationParameter:
            param = value.value<QPair<int, double>>();

            if(param.first == Step::Absolute) {
                currentStep.stabilisationType = param.first;
                currentStep.stabilisationParamAbsolute = Measurements::Pressure::fromKiloPascals(param.second);
            } else if(param.first == Step::Relative) {
                currentStep.stabilisationType = param.first;
                currentStep.stabilisationParamRelative = param.second / 100.0;
            }

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

QVariant AdvancedSaturationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    

    if((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
        const QStringList headers = {
            tr("Всестороннее\nдавление, σ3"),
            tr("Дифференциальное\nдавление, σ3 - u"),
            tr("Критерий перехода\nк следующей ступени"),
            tr("Параметр\nстабилизации"),
            tr("Время")
        };

        
        return headers.at(section);

    } else {
        
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    
}

Qt::ItemFlags AdvancedSaturationModel::flags(const QModelIndex &index) const
{
    

    Q_UNUSED(index)

    

    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

int AdvancedSaturationModel::addStep(const Step &step)
{
    const auto row = m_steps.size();
    beginInsertRows(QModelIndex(), row, row);
    Step newStep;
    m_steps.append(step);
    endInsertRows();
    return m_steps.size();
}

int AdvancedSaturationModel::insertStep(const QModelIndex &index)
{
    

    const auto row = index.row() + 1;

    beginInsertRows(QModelIndex(), row, row);
    m_steps.insert(row, Step());
    endInsertRows();

    

    return row;
}

int AdvancedSaturationModel::removeStep(const QModelIndex &index)
{
    

    const auto row = index.row();

    if((row >= 0) && (row < m_steps.count())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_steps.removeAt(row);
        endRemoveRows();
    }

    if (row == m_steps.count())
    {
        
        return row - 1;
    }
    else
    {
        
        return row;
    }

    
}

int AdvancedSaturationModel::moveStep(const QModelIndex &index, int moveAmount)
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

void AdvancedSaturationModel::setStep(const QModelIndex &index, const AdvancedSaturationModel::Step &step)
{
    

    m_steps.replace(index.row(), step);
    emit dataChanged(QModelIndex(), QModelIndex());

    
}

int AdvancedSaturationModel::duplicateStep(const QModelIndex &index)
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
