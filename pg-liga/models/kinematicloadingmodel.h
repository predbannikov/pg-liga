#ifndef KINEMATICLOADINGMODEL_H
#define KINEMATICLOADINGMODEL_H

#include <QAbstractTableModel>

#include "measurements.h"

class KinematicLoadingModel : public QAbstractTableModel
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

    KinematicLoadingModel(QObject *parent = nullptr);

    void setLoadingSpeed(Measurements::Speed speed);

    Measurements::Speed loadingSpeed() const {
        if (m_loadingSpeed.millimetresPerMinute() > 5.0) {
            qDebug() << Q_FUNC_INFO << QString("m_loadingSpeed=%1, скорость будет занижена до 5.0");
            return Measurements::Speed::fromMillimetresPerMinute(5.0);
        } else {
            return m_loadingSpeed;
        }
    }
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
    Measurements::Speed m_loadingSpeed = Measurements::Speed::fromMillimetresPerMinute(0.2);
    Measurements::Length m_targetDeformAbsolute = Measurements::Length::fromMillimetres(1);
    double m_targetDeformRelative = 0.15;
    int m_deformType = DeformRelative;
};

#endif // KINEMATICLOADINGMODEL_H
