#ifndef STEPPEDPRESSURISEMODEL_H
#define STEPPEDPRESSURISEMODEL_H

#include "measurements.h"
#include "abstractsteppedmodel.h"

// TODO Unify code with other Step models and thus eliminate the copypasta

class SteppedPressuriseModel : public AbstractSteppedModel
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

        CriterionType criterion = Stabilisation;
        StabilisationType stabilisationType = Absolute;

        Measurements::Pressure stabilisationParamAbsolute = Measurements::Pressure::fromKiloPascals(3);
        double stabilisationParamRelative = 0.1;

        Measurements::TimeInterval timeOfCriterionTime = Measurements::TimeInterval::fromHMS(0, 15, 0);
        void setCriterion(QString criter) {
            if (criter == "Stabilisation") {
                criterion = Stabilisation;
            } else if (criter == "Duration") {
                criterion = Duration;
            } else if (criter == "Manual") {
                criterion = Manual;
            }
        }
        QString criterionToStr() {
            switch(criterion) {
            case SteppedPressuriseModel::Step::Manual:
                return "Manual";
            case SteppedPressuriseModel::Step::Duration:
                return "Duration";
            case SteppedPressuriseModel::Step::Stabilisation:
                return "Stabilisation";
            }
            return "unknown_criterionType";
        }
        QString stabilisationTypeToStr() {
            switch (stabilisationType) {
            case SteppedPressuriseModel::Step::Absolute:
                return "Absolute";
            case SteppedPressuriseModel::Step::Relative:
                return "Relative";
            }
            return "unknown_stabilizationType";
        }
        QString stabilisationParamToStr() {
            switch (stabilisationType) {
            case SteppedPressuriseModel::Step::Absolute:
                return stabilisationParamAbsolute.kiloPascalsStr();
            case SteppedPressuriseModel::Step::Relative:
                return QString::number(stabilisationParamRelative);
            }
            return "unknown_stabilisationParam";
        }
        QString timeOfCriterionTimeToStr() {
            return timeOfCriterionTime.toString();
        }
        QJsonObject toJson() {
            QJsonObject jobj;
            jobj["target"] = QString::number(cellPressure.pascals());
            jobj["criterionType"] = criterionToStr();
            jobj["stabilisationType"] = stabilisationTypeToStr();
            jobj["stabilisationParam"] = stabilisationParamToStr();
            jobj["timeOfCriterionTime"] = timeOfCriterionTimeToStr();
            return jobj;
        }
    };

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
    QJsonObject serializModel();

    /* Getters */
    Step step(int idx) const { return m_steps.at(idx); }
    int stepCount() const { return m_steps.size(); }

    Measurements::Pressure getOverPressureCellVolumeter() { return m_overPressureCellVolumeter; }
    Measurements::Pressure getOverPressurePoreVolumeter() { return m_overPressurePoreVolumeter; }

private:
    QList<Step> m_steps;
    Measurements::Pressure m_overPressureCellVolumeter;
    Measurements::Pressure m_overPressurePoreVolumeter;
};

#endif // STEPPEDPRESSURISEMODEL_H
