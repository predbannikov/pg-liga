#ifndef DEFORMMODEL_H
#define DEFORMMODEL_H

#include "measurements.h"
#include "abstractsteppedmodel.h"

class DeformModel : public AbstractSteppedModel
{
    Q_OBJECT
public:
    enum Column {
        ColumnCritery,
        ColumnParamStabilization,
        ColumnTime,
        ColumnCount
    };

    enum DeformType {
        DeformAbsolute,
        DeformRelative,
        PressureMode
    };


    struct Step {
        enum CriterionType {
            Manual,
            Duration,
            Stabilisation
        };

        enum StabilisationType {
            Absolute,
            Relative,
            Pressure
        };


        int criterion = Manual;
        int stabilisationType = Absolute;

        Measurements::Length stabilisationParamAbsolute = Measurements::Length::fromMillimetres(1.0);
        double stabilisationParamRelative = 0.15;
        Measurements::Pressure stabilisationParamPressure = Measurements::Pressure::fromKiloPascals(5.0);

        Measurements::TimeLongInterval durationTime = Measurements::TimeLongInterval::fromHMS(0, 1, 0);
        Measurements::TimeLongInterval stabilisationTime = Measurements::TimeLongInterval::fromHMS(0, 1, 0);
    };

    DeformModel(QObject *parent = nullptr);

    /* Reimplemented AbstractTestModel functions */
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    int insertStep(const QModelIndex &index = QModelIndex()) override;
    int removeStep(const QModelIndex &index = QModelIndex()) override;
    int moveStep(const QModelIndex &index, int moveAmount) override;

    void setStep(const QModelIndex &index, const Step &step);
    int duplicateStep(const QModelIndex &index = QModelIndex()) override;

    /* Getters */
    Step step(int idx) const { return m_steps.at(idx); }
    int stepCount() const { return m_steps.size(); }
    bool isLast(int idx) const;

private:
    QStringList headers() const {
        return {
            tr("Критерий перехода\nк следующей ступени"),
            tr("Параметр стабилизации"),
            tr("Время")
        };
    };

    QList<Step> m_steps;
};


#endif // DEFORMMODEL_H
