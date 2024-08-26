#ifndef RELAXATIONLOADINGMODEL_H
#define RELAXATIONLOADINGMODEL_H

#include "measurements.h"
#include "abstractsteppedmodel.h"
//#include "specimen.h"

//class Specimen;

class RelaxationLoadingModel : public AbstractSteppedModel
{
public:

    enum Column {
        StepParam,  //could be deform, could be pressure. weighted delegate?
        CriterionType,  //just selector
        Parameter,      //endstep parameter
        Time,

        ColumnCount
    };

    struct Step {
        enum type {
            Deformation,
            Pressure
        };

        enum Type {
            Manual,
            Duration,
            Stabilisation //pressure speed stabilisation
        };

        Measurements::Pressure pressure = Measurements::Pressure::fromKiloPascals(100.0);  //it's bare vertical pressure
        double relativePressure = 25.0;

        Measurements::Length deform = Measurements::Length::fromMillimetres(0.1);          //it's deform
        int type = Deformation;

        //double deformCriterion = 0.15;
        Measurements::TimeInterval durationTime = Measurements::TimeInterval::fromHMS(0, 15, 0);
        Measurements::TimeInterval stabilisationTime = Measurements::TimeInterval::fromHMS(0, 1, 0);
        Measurements::Pressure speedCritetion = Measurements::Pressure::fromKiloPascals(70.0);      //pressure speed! kPa/min
        int criterion = Manual;
    };

    RelaxationLoadingModel(QObject *parent = nullptr);

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

    //int insertInitialStep(Measurements::Pressure speed, const QModelIndex &index = QModelIndex());

    void setStep(const QModelIndex &index, const Step &step);

    /* Getters */
    Step step(int idx) const { return m_steps.at(idx); }
    int stepCount() const { return m_steps.size(); }

private:
    QList<Step> m_steps;

   // Step *m_stepdata;
   //Specimen *m_specimen;
};

#endif // RELAXATIONLOADINGMODEL_H
