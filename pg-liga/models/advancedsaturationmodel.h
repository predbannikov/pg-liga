#ifndef ADVANCEDSATURATIONMODEL_H
#define ADVANCEDSATURATIONMODEL_H

#include "measurements.h"
#include "abstractsteppedmodel.h"

class AdvancedSaturationModel : public AbstractSteppedModel
{
    Q_OBJECT

public:
    enum Column {
        CellPressure,
        PorePressure,
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

        Measurements::Pressure cellPressure = Measurements::Pressure::fromKiloPascals(100);
        Measurements::Pressure diffPressure = Measurements::Pressure::fromKiloPascals(0);

        int criterion = Stabilisation;
        int stabilisationType = Absolute;

        Measurements::Pressure stabilisationParamAbsolute = Measurements::Pressure::fromKiloPascals(3);
        double stabilisationParamRelative = 0.1;

        Measurements::TimeInterval durationTime = Measurements::TimeInterval::fromHMS(0, 15, 0);
        Measurements::TimeInterval stabilisationTime = Measurements::TimeInterval::fromHMS(0, 15, 0);

        void setCriterion(QString criter) {
            if (criter == "Stabilisation") {
                criterion = Stabilisation;
            } else if (criter == "Duration") {
                criterion = Duration;
            } else if (criter == "Manual") {
                criterion = Manual;
            }
        }
    };

    AdvancedSaturationModel(Measurements::Pressure overPressureCellVolumeter, Measurements::Pressure overPressurePoreVolumeter, QObject *parent = nullptr);

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
    int addStep(const Step &step);
    void setStep(const QModelIndex &index, const Step &step);
    int duplicateStep(const QModelIndex &index = QModelIndex()) override;

    /* Getters */
    Step step(int idx) const { return m_steps.at(idx); }
    int stepCount() const { return m_steps.size(); }

    Measurements::Pressure getOverPressureCellVolumeter() { return m_overPressureCellVolumeter; }
    Measurements::Pressure getOverPressurePoreVolumeter() { return m_overPressurePoreVolumeter; }

private:
    QList<Step> m_steps;
    const Measurements::Pressure m_overPressureCellVolumeter;
    const Measurements::Pressure m_overPressurePoreVolumeter;
};

#endif // ADVANCEDSATURATIONMODEL_H
