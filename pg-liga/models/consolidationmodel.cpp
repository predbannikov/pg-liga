#include <QDebug>
#include "consolidationmodel.h"

ConsolidationModel::ConsolidationModel(QObject *parent):
    AbstractSteppedModel(parent)
{
    
    
}

int ConsolidationModel::rowCount(const QModelIndex &parent) const
{
    

    if(parent.isValid()) {
        
        return 0;
    } else {
        
        return m_steps.size();
    }

    
}

int ConsolidationModel::columnCount(const QModelIndex &parent) const
{
    

    if(parent.isValid()) {
        
        return 0;
    } else {
        
        return ColumnCount;
    }

    
}

QVariant ConsolidationModel::data(const QModelIndex &index, int role) const //отображение данных
{
    

    const auto row = index.row();
    const auto col = index.column();
    const auto currentStep = step(row);

    if((role == Qt::EditRole) || (role == Qt::DisplayRole)) {
        switch(col) {
        case CellPressure:                              //проходим ячейку всестороннего давления
            
            return currentStep.cellPressure.kiloPascals();
        case EndCriterion:                              //проходим ячейку критерия завершения ступени
            if(isLast(row)) {
            
                return currentStep.criterion;
            } else {
                
                return QVariant();
            }
        case StabilisationParameter:                    //Проходим ячейку значения параметра стабилизации
            if (isLast(row)) {                          //параметр нам нужен только на последней ступени
                switch(currentStep.criterion) {
                case Step::Stabilisation:
                    
                    return currentStep.stabilisationParam * 100.0;
                case Step::Duration:
                    
                    return QVariant();
                case Step::Manual:
                    
                    return QVariant();
                default:
                    return QVariant();
                    
                }
            } else {
                
                return QVariant();
            }
        case Time:                                      //Проходим ячейку значения времени
            if(!isLast(row)) {
                
                return QVariant::fromValue(currentStep.durationTime);
            } else {
                switch(currentStep.criterion) {
                case Step::Duration: {
                    
                    return QVariant::fromValue(currentStep.endTime);
                }
                case Step::Stabilisation: {
                    
//                    return QVariant::fromValue(currentStep.stabilisationTime);
                }
                case Step::Manual: {
                    
                    return QVariant::fromValue(tr("100% консолидация"));
                    }
                }
            }

        default:
            
            return QVariant();
        }
    } else {
        
        return QVariant();
    }

    
}

bool ConsolidationModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    

    const auto underpressure = Measurements::Pressure::fromPascals(0);
    const auto overpressure = Measurements::Pressure::fromKiloPascals(2800); //TODO make it refer to volumeter.h without breaking rules of using references

    const auto understabilisationparam = 0.0;

    const auto overdurationtime = Measurements::TimeLongInterval::fromDHM(0, 12, 1);

    const auto understabilisationtime = Measurements::TimeLongInterval::fromDHM(0, 0, 29);
    const auto overstabilisationtime = Measurements::TimeLongInterval::fromDHM(0, 3, 21);

    const auto underendtime = Measurements::TimeLongInterval::fromDHM(0, 0, 29);

    if(role == Qt::EditRole || role == Qt::DisplayRole) {
        const auto row = index.row();
        const auto col = index.column();
        auto currentStep = step(row);

        switch(col) {
        case CellPressure:              //задаем всестороннее давление
            if(Measurements::Pressure::fromKiloPascals(value.toDouble()) == underpressure)
                 currentStep.cellPressure = Measurements::Pressure::fromKiloPascals(50);
            else if(Measurements::Pressure::fromKiloPascals(value.toDouble()) >= overpressure)
                 currentStep.cellPressure = Measurements::Pressure::fromKiloPascals(2795);
            else currentStep.cellPressure = Measurements::Pressure::fromKiloPascals(value.toDouble());
            break;
        case EndCriterion:                          //задаём параметр завершения ступени
            currentStep.criterion = value.toInt();

            switch(currentStep.criterion) {         //нужно сбрасывать параметры от предыдущего критерия, чтобы добиться правильного срабатывания проверки ступеней
            case Step::Manual: {
                currentStep.endTime = Measurements::TimeLongInterval::fromDHM(0, 2, 30);
                currentStep.stabilisationParam = 0.05;
//                currentStep.stabilisationTime = Measurements::TimeLongInterval::fromDHM(0, 0, 30);
                break;
            }
            case Step::Stabilisation: {
               currentStep.endTime = Measurements::TimeLongInterval::fromDHM(0, 2, 30);
               break;
            }
            case Step::Duration: {
               currentStep.stabilisationParam = 0.05;
//               currentStep.stabilisationTime = Measurements::TimeLongInterval::fromDHM(0, 0, 30);
               break;
            }
            }
            break;

        case StabilisationParameter:    //задаём параметр стабилизации
            if (value.toDouble() == understabilisationparam) currentStep.stabilisationParam = 0.05;
            else currentStep.stabilisationParam = value.toDouble() / 100.0;
               break;

        case Time:                      //задаём параметр времени
            if(!isLast(row)) {
                if (value.value<Measurements::TimeLongInterval>() >= overdurationtime)
                     currentStep.durationTime = Measurements::TimeLongInterval::fromDHM(0, 12, 0);
                else currentStep.durationTime = value.value<Measurements::TimeLongInterval>();
            } else {
                switch(currentStep.criterion) {
                case Step::Manual: {
                    break;
                }
                case Step::Stabilisation: {
//                    if (value.value<Measurements::TimeLongInterval>() <= understabilisationtime)
//                        currentStep.stabilisationTime = Measurements::TimeLongInterval::fromDHM(0, 0, 30);
//                    else
//                    if (value.value<Measurements::TimeLongInterval>() >= overstabilisationtime)
//                        currentStep.stabilisationTime = Measurements::TimeLongInterval::fromDHM(0, 3, 20);
//                   else currentStep.stabilisationTime = value.value<Measurements::TimeLongInterval>();
                   break;
                }
                case Step::Duration: {
                   if (value.value<Measurements::TimeLongInterval>() < underendtime)
                        currentStep.endTime = Measurements::TimeLongInterval::fromDHM(0, 0, 30);
                   else currentStep.endTime = value.value<Measurements::TimeLongInterval>();
                   break;
                }
                }
            }
            break;

        default:
            
            return false;
        }

        setStep(index, currentStep);
        
        return true;

    } else {
        
        return false;
    }

    
}

QVariant ConsolidationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    

    if((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
        const QStringList headers = {
            tr("Всестороннее давление,\nσ3"),
            tr("Критерий завершения\n ступени"),
            tr("Параметр\nстабилизации"),
            tr("Время")
        };

        
        return headers.at(section);

    } else {
        
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    
}

Qt::ItemFlags ConsolidationModel::flags(const QModelIndex &index) const
{
    

    Q_UNUSED(index)

    const auto row = index.row();
    const auto column = index.column();
    auto currentStep = step(row);

    if (!isLast(row) && (column == EndCriterion || column == StabilisationParameter))
    {
        
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    } else if (isLast(row)) {
        switch(currentStep.criterion){
        case Step::Stabilisation: {
            
            return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
        }
        case Step::Duration: {
            if (column == StabilisationParameter) return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
            else if (column == Time) return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
        }
        case Step::Manual: {
            if (column == StabilisationParameter || column == Time) return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
        }
        default: {
            
            return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
        }
        }
    } 
    else 
    {
        
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }

    
}

int ConsolidationModel::insertStep(const QModelIndex &index)
{
    

    const auto row = index.row() + 1;

    beginInsertRows(QModelIndex(), row, row);
    m_steps.insert(row, Step());
    endInsertRows();

    

    return row;
}

int ConsolidationModel::removeStep(const QModelIndex &index)
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

int ConsolidationModel::moveStep(const QModelIndex &index, int moveAmount)
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

void ConsolidationModel::setStep(const QModelIndex &index, const ConsolidationModel::Step &step)
{
    

    m_steps.replace(index.row(), step);
    emit dataChanged(QModelIndex(), QModelIndex());

    
}

int ConsolidationModel::addStep(const Step &step)
{
    const auto row = m_steps.size();
    beginInsertRows(QModelIndex(), row, row);
    Step newStep;
    m_steps.append(step);
    endInsertRows();
    return m_steps.size();
}

int ConsolidationModel::duplicateStep(const QModelIndex &index)
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

bool ConsolidationModel::isLast(int idx) const
{
    
    

    return idx == m_steps.size() - 1;
}
