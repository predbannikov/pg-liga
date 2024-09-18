#ifndef STEPPEDPRESSURISEMODEL_H
#define STEPPEDPRESSURISEMODEL_H

#include "measurements.h"
#include "abstractsteppedmodel.h"

// TODO Unify code with other Step models and thus eliminate the copypasta

class SteppedPressuriseModel : public AbstractSteppedModel
{
    Q_OBJECT

public:
    SteppedPressuriseModel(QObject *parent = nullptr);
    /* Reimplemented AbstractTestModel functions */
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /* Data editing API */
    int addStep(const Step &step);
    int insertStep(const QModelIndex &index = QModelIndex()) override;
    int removeStep(const QModelIndex &index = QModelIndex()) override;
    int moveStep(const QModelIndex &index, int moveAmount) override;
    void setStep(const QModelIndex &index, const Step &step);
    int duplicateStep(const QModelIndex &index = QModelIndex()) override;

    /* Getters */
    Step step(int idx) const { return m_steps.at(idx); }
    int stepCount() const { return m_steps.size(); }

    Measurements::Pressure getOverPressureCellVolumeter() { return m_overPressureCellVolumeter; }
    Measurements::Pressure getOverPressurePoreVolumeter() { return m_overPressurePoreVolumeter; }

private:

    Measurements::Pressure m_overPressureCellVolumeter;
    Measurements::Pressure m_overPressurePoreVolumeter;
};

#endif // STEPPEDPRESSURISEMODEL_H
