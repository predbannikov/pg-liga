#ifndef CONSOLIDATIONALTMODEL_H
#define CONSOLIDATIONALTMODEL_H

#include "measurements.h"
#include "abstractsteppedmodel.h"

class ConsolidationAltModel : public AbstractSteppedModel
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
            Duration,
            Stabilisation
        };

        enum StabilisationType {
            Absolute,
            Relative
        };

        Measurements::Pressure cellPressure = Measurements::Pressure::fromKiloPascals(50);

        int criterion = Stabilisation;
        int stabilisationType = Relative;

        Measurements::Volume stabilisationParamAbsolute;
        double stabilisationParamRelative = 0.0015;

        Measurements::TimeInterval durationTime = Measurements::TimeInterval::fromHMS(0, 1, 0);
        Measurements::TimeInterval stabilisationTime = Measurements::TimeInterval::fromHMS(0, 1, 0);
    };

    ConsolidationAltModel(QObject *parent = nullptr);
    bool isLast(int idx) const;    // добавлено по аналогии с классом ConsolidationOperation 23.03.2022 г. jasha

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
    int duplicateStep(const QModelIndex &index = QModelIndex()) override;
    int addStep(const Step &step);

    /* Getters */
    Step step(int idx) const { return m_steps.at(idx); }
    int stepCount() const { return m_steps.size(); }

private:
    QList<Step> m_steps;
};

#endif // CONSOLIDATIONALTMODEL_H
