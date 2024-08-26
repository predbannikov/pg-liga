#ifndef CONSOLIDATIONMODEL_H
#define CONSOLIDATIONMODEL_H

#include "measurements.h"
#include "abstractsteppedmodel.h"

class ConsolidationModel : public AbstractSteppedModel
{
    Q_OBJECT

public:
    enum Column {
        CellPressure,
        EndCriterion,
        StabilisationParameter,
        Time,

        ColumnCount
    };

    struct Step {
        enum CriterionType {
            Manual,
            Stabilisation,
            Duration
        };

        Measurements::Pressure cellPressure = Measurements::Pressure::fromKiloPascals(50);

        int criterion = Duration;
        double stabilisationParam = 0.05;

        Measurements::TimeLongInterval durationTime = Measurements::TimeLongInterval::fromDHM(0, 0, 5);             //время проведения ступени
//        Measurements::TimeLongInterval stabilisationTime = Measurements::TimeLongInterval::fromDHM(0, 0, 30);       //delta t - стабилизация за период времени
        Measurements::TimeLongInterval endTime = Measurements::TimeLongInterval::fromDHM(0, 2, 30);                 // T - безусловное завершение консолидации, без проверки деформации
    };

    ConsolidationModel(QObject *parent = nullptr);
    bool isLast(int idx) const;

    /* Reimplemented AbstractTestModel functions */
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /* Data editing API */
    int insertStep(const QModelIndex &index = QModelIndex()) override;
    int removeStep(const QModelIndex &index = QModelIndex()) override;
    int moveStep(const QModelIndex &index, int moveAmount) override;
    void setStep(const QModelIndex &index, const Step &step);
    int addStep(const Step &step);
    int duplicateStep(const QModelIndex &index = QModelIndex()) override;

    /* Getters */
    Step step(int idx) const { return m_steps.at(idx); }
    int stepCount() const { return m_steps.size(); }

private:
    QList<Step> m_steps;
};

#endif // CONSOLIDATIONMODEL_H
