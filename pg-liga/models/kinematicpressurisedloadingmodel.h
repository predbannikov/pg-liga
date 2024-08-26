#ifndef KINEMATICPRESSURISEDLOADINGMODEL_H
#define KINEMATICPRESSURISEDLOADINGMODEL_H

#include <QAbstractTableModel>

#include "measurements.h"

class KinematicPressurisedLoadingModel : public QAbstractTableModel
{
public:
    enum Column {
        PressureSpeed,
        TargetDeform,
        ColumnCount
    };

    enum DeformType {
        DeformAbsolute,
        DeformRelative
    };

    KinematicPressurisedLoadingModel(QObject *parent = nullptr);

    //void setLoadingPressureSpeed(Measurements::PressureSpeed speed);
    //void setPressureSpeed(Measurements::Pressure pressure);

    //Measurements::PressureSpeed loadingSpeedP () const { return m_loadingSpeedp; }
    Measurements::Pressure loadingSpeed() const { return m_pressureSpeed; }


    Measurements::Length targetDeformAbsolute() const { return m_targetDeformAbsolute; }
    double targetDeformRelative() const { return m_targetDeformRelative; }
    int deformType() const { return m_deformType; }

    /* Reimplemented AbstractTestModel functions */
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    //Measurements::PressureSpeed m_loadingSpeedp = Measurements::PressureSpeed::fromKilopascalsPerSecond(1000.0);
    Measurements::Pressure m_pressureSpeed = Measurements::Pressure::fromMegaPascals(0.1);
    Measurements::Length m_targetDeformAbsolute = Measurements::Length::fromMillimetres(1);
    double m_targetDeformRelative = 0.1;
    int m_deformType = DeformRelative;
};
#endif // KINEMATICPRESSURISEDLOADINGMODEL_H
