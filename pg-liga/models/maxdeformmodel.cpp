#include "maxdeformmodel.h"

#include <QDebug>
#include <QPair>

MaxDeformModel::MaxDeformModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

int MaxDeformModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    else
        return 1;
}

int MaxDeformModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    else
        return ColumnCount;
}

QVariant MaxDeformModel::data(const QModelIndex &index, int role) const
{
    const auto column = index.column();
    if(role == Qt::EditRole || role == Qt::DisplayRole) {
        switch(column) {
        case ColumnValue:
            if(m_deformType == DeformAbsolute) {
                return QVariant::fromValue(QPair<int, double>(m_deformType, m_targetDeformAbsolute.millimetres()));
            } else if(m_deformType == DeformRelative) {
                return QVariant::fromValue(QPair<int, double>(m_deformType, m_targetDeformRelative * 100.0));
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

bool MaxDeformModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::EditRole || role == Qt::DisplayRole) {
        const auto column = index.column();
        QPair<int, double> param;
        switch(column) {
        case ColumnValue:
            param = value.value<QPair<int,double>>();
            m_deformType = param.first;
            if(m_deformType == DeformAbsolute) {
                m_targetDeformAbsolute = Measurements::Length::fromMillimetres(param.second);
            } else if(m_deformType == DeformRelative) {
                m_targetDeformRelative = param.second / 100.0;
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

QVariant MaxDeformModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
        const QStringList compressionKinematicHeaders = {
//            tr("Общий критерий для всего эксперимента,\nv"),
            tr("Общие критерии действующие на протяжении\n всего времени эксперимента"),
//            tr("Целевая деформация,\nε")
            tr("Значение")
        };
        return compressionKinematicHeaders.at(section);
    } else {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
}

Qt::ItemFlags MaxDeformModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}
