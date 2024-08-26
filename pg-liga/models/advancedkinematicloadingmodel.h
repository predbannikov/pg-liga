#ifndef ADVANCEDKINEMATICLOADINGMODEL_H
#define ADVANCEDKINEMATICLOADINGMODEL_H

#include "measurements.h"
#include "abstractsteppedmodel.h"

// TODO Unify code with other stepped models and thus eliminate the copypasta
//class Specimen;

class AdvancedKinematicLoadingModel : public AbstractSteppedModel
{
public:
    enum Column {
        LoadingSpeed,
        CriterionType,
        Parameter,

        ColumnCount
    };

    struct Step {
        enum Type {
            Pressure,
            Deformation
        };

        Measurements::Speed loadingSpeed = Measurements::Speed::fromMillimetresPerMinute(0.03);
        Measurements::Pressure pressureCriterion = Measurements::Pressure::fromKiloPascals(100.0);


        double deformCriterion = 0.15;
        int type = Pressure;
    };

    AdvancedKinematicLoadingModel(QObject *parent = nullptr);


    /* Reimplemented AbstractTestModel functions */
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /* Reimplemented AbstractSteppedModel functions */
    int insertStep(const QModelIndex &index = QModelIndex()) override;
    int removeStep(const QModelIndex &index = QModelIndex()) override;
    int moveStep(const QModelIndex &index, int moveAmount) override;
    int duplicateStep(const QModelIndex &index = QModelIndex()) override;

    //int insertInitialStep(Measurements::Speed speed, const QModelIndex &index = QModelIndex());

    void setStep(const QModelIndex &index, const Step &step);

    /* Getters */
    Step step(int idx) const { return m_steps.at(idx); }
    int stepCount() const { return m_steps.size(); }

private:
    QList<Step> m_steps;

    //Step *m_stepdata;
    //Specimen *m_specimen;
};

#endif // ADVANCEDKINEMATICLOADINGMODEL_H
