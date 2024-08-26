#ifndef MAXPRESSUREMODEL_H
#define MAXPRESSUREMODEL_H

#include <QAbstractTableModel>
#include "measurements.h"

class MaxPressureModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        ColumntName,
        ColumnValue,
        ColumnCount
    };

    enum PressureType {
        PressureAbsolute
    };

//    MaxPressureModel(QString nameCriterion = "Критерий давления завершения эксперимента", QObject *parent = nullptr);
    MaxPressureModel(QString nameCriterion, QObject *parent = nullptr);


    Measurements::Pressure targetPressureAbsolute() const { return m_targetPressureAbsolute; }
    int pressureType() const { return m_pressureType; }

    /* Reimplemented AbstractTestModel functions */
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    void setPressure(double pressurekPa);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    Measurements::Pressure m_targetPressureAbsolute = Measurements::Pressure::fromKiloPascals(150);
    int m_pressureType = PressureAbsolute;
    QString m_nameCriterion;
};

#endif // MAXPRESSUREMODEL_H
