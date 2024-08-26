#include "kinematicloadingmodel.h"

#include <QDebug>
#include <QPair>

KinematicLoadingModel::KinematicLoadingModel(QObject *parent):
    QAbstractTableModel(parent)
{
    
    
}

/*Example of editing model outside. But it's better to use setData() func.*/
void KinematicLoadingModel::setLoadingSpeed(Measurements::Speed speed)
{
    

    m_loadingSpeed = speed;
    emit dataChanged(QModelIndex(), QModelIndex()); //example!

    
}


int KinematicLoadingModel::rowCount(const QModelIndex &parent) const
{
    

    if(parent.isValid()) {
        
        return 0;
    } else {
        
        return 1;
    }

    
}

int KinematicLoadingModel::columnCount(const QModelIndex &parent) const
{
    

    if(parent.isValid()) {
        
        return 0;
    } else {
        
        return ColumnCount;
    }

    
}

QVariant KinematicLoadingModel::data(const QModelIndex &index, int role) const
{
    

    const auto column = index.column();

    if(role == Qt::EditRole || role == Qt::DisplayRole) {
        switch(column) {
        case PressureSpeed:
            
            return m_loadingSpeed.millimetresPerMinute();

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

bool KinematicLoadingModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    

    const auto overloadingspeed = Measurements::Speed::fromMillimetresPerMinute(20.0);
    const auto maxDeformation = Measurements::Length::fromMillimetres(51); //TODO INSTR TYPE || actual liga model maxdeform 5t - 50mm 25%, 1t - 25mm ~33%

    if(role == Qt::EditRole || role == Qt::DisplayRole) {
        const auto column = index.column();

        QPair<int, double> param;

        switch(column) {
        case PressureSpeed:
            if (Measurements::Speed::fromMillimetresPerMinute(value.toDouble()) >= overloadingspeed
                    || Measurements::Speed::fromMillimetresPerMinute(value.toDouble()) == Measurements::Speed::fromMillimetresPerMinute(0.0)) {
                qDebug() <<  Q_FUNC_INFO << QString("value=%1, слишком высокая скорость, по умолчанию будет 0.2");
                m_loadingSpeed = Measurements::Speed::fromMillimetresPerMinute(0.2);
            } else {
                m_loadingSpeed = Measurements::Speed::fromMillimetresPerMinute(value.toDouble());
            }
            break;

        case TargetDeform:
            param = value.value<QPair<int,double>>();

            m_deformType = param.first;

            if(m_deformType == DeformAbsolute) {
                if (Measurements::Length::fromMillimetres(param.second) >= maxDeformation)
                     m_targetDeformAbsolute = Measurements::Length::fromMillimetres(15);
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

QVariant KinematicLoadingModel::headerData(int section, Qt::Orientation orientation, int role) const
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

Qt::ItemFlags KinematicLoadingModel::flags(const QModelIndex &index) const
{
    

    Q_UNUSED(index)

    

    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}
