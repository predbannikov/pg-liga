#ifndef MAXDEFORMMODEL_H
#define MAXDEFORMMODEL_H

#include <QAbstractTableModel>
#include "measurements.h"

class MaxDeformModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        ColumntName,
        ColumnValue,
        ColumnCount
    };

    enum DeformType {
        DeformAbsolute,
        DeformRelative
    };

    MaxDeformModel(QObject *parent = nullptr);


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
    Measurements::Length m_targetDeformAbsolute = Measurements::Length::fromMillimetres(15);
    double m_targetDeformRelative = 0.15;
    int m_deformType = DeformRelative;
    QString m_nameCriterion = tr("Критерий деформации завершения эксперимента");
};

#endif // MAXDEFORMMODEL_H
