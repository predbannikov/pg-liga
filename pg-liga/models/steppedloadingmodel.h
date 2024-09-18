#ifndef COMPRESSIONSTATICMODEL_H
#define COMPRESSIONSTATICMODEL_H

#include "measurements.h"
#include "abstractsteppedmodel.h"

// TODO Unify code with other Step models and thus eliminate the copypasta

class SteppedLoadingModel : public AbstractSteppedModel
{
    Q_OBJECT

public:

    SteppedLoadingModel(QObject *parent = nullptr);

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
    int duplicateStep(const QModelIndex &index = QModelIndex()) override;

    void setStep(const QModelIndex &index, const Step &step);

    /* Getters */
    Step step(int idx) const { return m_steps.at(idx); }
    int stepCount() const { return m_steps.size(); }

    bool isLast(int idx) const;

private:
    virtual QStringList headers() const = 0;

};

class StaticCompressionModel : public SteppedLoadingModel
{
public:
    StaticCompressionModel(QObject *parent = nullptr):
        SteppedLoadingModel(parent) {
        m_type = "staticCompressionModel";
    }


private:
    QStringList headers() const override {
        return {
            tr("Вертикальное давление,\nσ1"),
            tr("Критерий перехода\nк следующей ступени"),
            tr("Параметр стабилизации"),
            tr("Время")
        };
    }
};

class StaticDeviatorModel : public SteppedLoadingModel
{
public:
    StaticDeviatorModel(QObject *parent = nullptr):
        SteppedLoadingModel(parent) {
        m_type = "staticDeviatorModel";
    }

private:
    QStringList headers() const override {
        return {
            tr("Осевое напряжение,\nσ1 - σ3"),
            tr("Критерий перехода\nк следующей ступени"),
            tr("Параметр стабилизации"),
            tr("Время")
        };
    }
};

class StaticShearModel : public SteppedLoadingModel
{
public:
    StaticShearModel(QObject *parent = nullptr):
        SteppedLoadingModel(parent) {
        m_type = "StaticShearModel";
    }

private:
    QStringList headers() const override {
        return {
            tr("Касательное напряжение"),
            tr("Критерий перехода\nк следующей ступени"),
            tr("Параметр стабилизации"),
            tr("Время")
        };
    }
};

#endif // COMPRESSIONSTATICMODEL_H
