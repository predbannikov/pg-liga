#include "maxpressuremodel.h"

#include <QDebug>
#include <QPair>

MaxPressureModel::MaxPressureModel(QString nameCriterion, QObject *parent)
    : QAbstractTableModel{parent},
      m_nameCriterion(nameCriterion)
{
}

int MaxPressureModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    else
        return 1;
}

int MaxPressureModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    else
        return ColumnCount;
}

QVariant MaxPressureModel::data(const QModelIndex &index, int role) const
{
    const auto column = index.column();
    if(role == Qt::EditRole || role == Qt::DisplayRole) {
        switch(column) {
        case ColumnValue:
            if(m_pressureType == PressureAbsolute) {
                return QVariant::fromValue(QPair<int, double>(m_pressureType, m_targetPressureAbsolute.kiloPascals()));
            } else {
                return QVariant();
            }
            break;
        case ColumntName:
            return m_nameCriterion;
        default:
            return QVariant();
        }
    } else {
        return QVariant();
    }
}

bool MaxPressureModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::EditRole || role == Qt::DisplayRole) {
        const auto column = index.column();
        QPair<int, double> param;
        switch(column) {
        case ColumnValue:
            param = value.value<QPair<int,double>>();
            m_pressureType = param.first;
            if(m_pressureType == PressureAbsolute) {
                m_targetPressureAbsolute = Measurements::Pressure::fromKiloPascals(param.second);
            } else {
                return false;
            }
            break;
        case ColumntName:
            return true;
        default:
            return false;
        }
        return true;
    } else {
        return false;
    }
}

void MaxPressureModel::setPressure(double pressurekPa)
{
    setData(index(0, 1), QVariant::fromValue(QPair<int, double>{PressureAbsolute, pressurekPa}));
}

QVariant MaxPressureModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
        const QStringList compressionRelaxationHeaders = {
//            tr("Общий критерий для всего эксперимента,\nv"),
            tr("Общие критерии действующие на протяжении\n всего времени эксперимента"),
//            tr("Целевое давление,\nε")
            tr("Значение")
        };
        return compressionRelaxationHeaders.at(section);
    } else {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
}

Qt::ItemFlags MaxPressureModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}
