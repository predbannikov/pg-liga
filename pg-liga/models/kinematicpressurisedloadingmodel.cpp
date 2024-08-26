#include "kinematicpressurisedloadingmodel.h"

#include <QPair>
#include <QDebug>

KinematicPressurisedLoadingModel::KinematicPressurisedLoadingModel(QObject *parent):
    QAbstractTableModel(parent)
{
    
    
}

/*Example of editing model outside. But it's better to use setData() func.*/
/*void KinematicPressurisedLoadingModel::setPressureSpeed(Measurements::Pressure speed)
{
    m_pressureSpeed = speed;
    emit dataChanged(QModelIndex(), QModelIndex()); //example!
}*/

int KinematicPressurisedLoadingModel::rowCount(const QModelIndex &parent) const
{
    

    if(parent.isValid()) {
        
        return 0;
    } else {
        
        return 1;
    }

    
}

int KinematicPressurisedLoadingModel::columnCount(const QModelIndex &parent) const
{
    

    if(parent.isValid()) {
        
        return 0;
    } else {
        
        return ColumnCount;
    }

    
}

QVariant KinematicPressurisedLoadingModel::data(const QModelIndex &index, int role) const
{
    

    const auto column = index.column();

    if(role == Qt::EditRole || role == Qt::DisplayRole) {
        switch(column) {
        case PressureSpeed:
            
            return m_pressureSpeed.kiloPascals();

        case TargetDeform:
            if(m_deformType == DeformAbsolute) {
                
                return QVariant::fromValue(QPair<int, double>(m_deformType, m_targetDeformAbsolute.millimetres()));
            } else if(m_deformType == DeformRelative) {
                
                return QVariant::fromValue(QPair<int, double>(m_deformType, m_targetDeformRelative * 100.0));
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

bool KinematicPressurisedLoadingModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    

    const auto overpressurespeed = Measurements::Pressure::fromKiloPascals(50001);
    const auto underPressurreSpeed = Measurements::Pressure::fromKiloPascals(100);
    const auto maxDeformation = Measurements::Length::fromMillimetres(51);

    if(role == Qt::EditRole || role == Qt::DisplayRole) {
        const auto column = index.column();

        QPair<int, double> param;

        switch(column) {
        case PressureSpeed:
            if (Measurements::Pressure::fromKiloPascals(value.toDouble()) >= overpressurespeed
                    || Measurements::Pressure::fromKiloPascals(value.toDouble()) < underPressurreSpeed)
                 m_pressureSpeed = Measurements::Pressure::fromMegaPascals(1.0);
            else m_pressureSpeed = Measurements::Pressure::fromKiloPascals(value.toDouble());
            break;

        case TargetDeform:
            param = value.value<QPair<int,double>>();

            m_deformType = param.first;

            if(m_deformType == DeformAbsolute) {
                if (Measurements::Length::fromMillimetres(param.second) >= maxDeformation)
                     m_targetDeformAbsolute = Measurements::Length::fromMillimetres(5);
                else m_targetDeformAbsolute = Measurements::Length::fromMillimetres(param.second);
            } else if(m_deformType == DeformRelative) {
                m_targetDeformRelative = param.second / 100.0;
            } else {
                
                return false;
            }

            break;

        default:
            
            return false;
        }

        
        return true;

    } else {
        
        return false;
    }

    
}

QVariant KinematicPressurisedLoadingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    

    if((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
        const QStringList compressionKinematicHeaders = {
            tr("Скорость нагружения,\nv"),
            tr("Целевая деформация,\nε")
        };

        
        return compressionKinematicHeaders.at(section);

    } else {
        
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    
}

Qt::ItemFlags KinematicPressurisedLoadingModel::flags(const QModelIndex &index) const
{
    

    Q_UNUSED(index)

    

    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}
