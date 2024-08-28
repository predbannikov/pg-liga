#include "deformmodel.h"

#include <QPair>
#include <QDebug>

DeformModel::DeformModel(QObject *parent)
    : AbstractSteppedModel{parent}
{

}

int DeformModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()) {

        return 0;
    } else {

        return m_steps.size();
    }
}

int DeformModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid()) {

        return 0;
    } else {

        return ColumnCount;
    }
}

QVariant DeformModel::data(const QModelIndex &index, int role) const
{
    const auto row = index.row();
    const auto col = index.column();
    const auto currentStep = step(row);

    if((role == Qt::EditRole) || (role == Qt::DisplayRole)) {
        switch(col) {
        case ColumnCritery:
            return currentStep.criterion;
        case ColumnParamStabilization:
            if(currentStep.criterion == Step::Stabilisation) {
                if(currentStep.stabilisationType == Step::Absolute) {
                    return QVariant::fromValue(QPair<int, double>(currentStep.stabilisationType, currentStep.stabilisationParamAbsolute.millimetres()));
                } else if(currentStep.stabilisationType == Step::Relative) {
                    return QVariant::fromValue(QPair<int, double>(currentStep.stabilisationType, currentStep.stabilisationParamRelative * 100.0));
                } else if(currentStep.stabilisationType == Step::Pressure) {
                    return QVariant::fromValue(QPair<int, double>(currentStep.stabilisationType, currentStep.stabilisationParamPressure.kiloPascals()));
                } else {
                    QVariant();
                }
            } else {
                return QVariant();
            }
        case ColumnTime:
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

bool DeformModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::EditRole || role == Qt::DisplayRole) {
        const auto row = index.row();
        const auto col = index.column();
        auto currentStep = step(row);
        QPair<int, double> param;

        switch(col) {
        case ColumnCritery:
            currentStep.criterion = value.toInt();
            break;

        case ColumnParamStabilization:
            param = value.value<QPair<int, double>>();

            if(param.first == Step::Absolute) {
                currentStep.stabilisationType = param.first;
                currentStep.stabilisationParamAbsolute = Measurements::Length::fromMillimetres(param.second);
            } else if(param.first == Step::Relative) {
                currentStep.stabilisationType = param.first;
                currentStep.stabilisationParamRelative = param.second / 100.0;
            } else if(param.first == Step::Pressure) {
                currentStep.stabilisationType = param.first;
                currentStep.stabilisationParamPressure = Measurements::Pressure::fromKiloPascals(param.second);
            } else {}
            break;

        case ColumnTime:
            switch(currentStep.criterion) {
            case Step::Duration:
                currentStep.durationTime = value.value<Measurements::TimeLongInterval>();
                break;

            case Step::Stabilisation:
                currentStep.stabilisationTime = value.value<Measurements::TimeLongInterval>();
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

QVariant DeformModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {

        return headers().at(section);
    } else {

        return QAbstractTableModel::headerData(section, orientation, role);
    }
}

Qt::ItemFlags DeformModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

int DeformModel::insertStep(const QModelIndex &index)
{
    const auto row = index.row() + 1;

    beginInsertRows(QModelIndex(), row, row);
    m_steps.insert(row, Step());
    endInsertRows();
    return row;
}

int DeformModel::removeStep(const QModelIndex &index)
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

int DeformModel::moveStep(const QModelIndex &index, int moveAmount)
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

void DeformModel::setStep(const QModelIndex &index, const DeformModel::Step &step)
{
    m_steps.replace(index.row(), step);
    emit dataChanged(QModelIndex(), QModelIndex());
}

int DeformModel::duplicateStep(const QModelIndex &index)
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

bool DeformModel::isLast(int idx) const
{
    return idx == m_steps.size() - 1;
}
