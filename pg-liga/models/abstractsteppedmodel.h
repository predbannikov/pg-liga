#ifndef ABSTRACTSTEPPEDMODEL_H
#define ABSTRACTSTEPPEDMODEL_H

#include <QVector>
#include <QAbstractTableModel>
#include <QJsonObject>
#include <measurements.h>

class AbstractSteppedModel : public QAbstractTableModel
{

public:
    AbstractSteppedModel(QObject *parent = nullptr):
        QAbstractTableModel(parent)
    {}

    /* Data editing API */
    virtual int insertStep(const QModelIndex &index = QModelIndex()) = 0;
    virtual int removeStep(const QModelIndex &index = QModelIndex()) = 0;
    virtual int moveStep(const QModelIndex &index, int moveAmount) = 0;
    virtual int duplicateStep(const QModelIndex &index = QModelIndex()) = 0;
    //    virtual double getTarget() = 0;

    Measurements::Pressure getOverPressureCellVolumeter() { return m_overPressureCellVolumeter; }
    Measurements::Pressure getOverPressurePoreVolumeter() { return m_overPressurePoreVolumeter; }

    QString getType() {return m_type;}

    QJsonObject serializModel()
    {
        QJsonObject jObj;
        int step = 0;
        for (; step < m_steps.size(); step++) {
            QString strStep = QString("step_%1").arg(step);
            jObj[strStep] = m_steps[step].toJson();
        }
        return jObj;
    }

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
        enum LoadMeasurement {
            Pressure,
            Newtones
        };

        Measurements::Pressure cellPressure; //target                 -> первый столбец
        Measurements::Pressure stabilisationParamAbsolute; // ...     -> третий столбец
        Measurements::TimeLongInterval timeOfCriterionTime; //Время       -> четвертый столбец
        CriterionType criterion; //Критерий (*, вермя, стабилизация)  -> второй столбец
        StabilisationType stabilisationType;//Тип стабилизации;       -> Чет не понятно
        double stabilisationParamRelative ;



        QString criterionToStr() {
            switch(criterion) {
            case AbstractSteppedModel::Step::Manual:
                return "Manual";
            case AbstractSteppedModel::Step::Duration:
                return "Duration";
            case AbstractSteppedModel::Step::Stabilisation:
                return "Stabilisation";
            }
            return "unknown_criterionType";
        }
        QString stabilisationTypeToStr() {
            switch (stabilisationType) {
            case AbstractSteppedModel::Step::Absolute:
                return "Absolute";
            case AbstractSteppedModel::Step::Relative:
                return "Relative";
            }
            return "unknown_stabilizationType";
        }
        QString stabilisationParamToStr() {
            switch (stabilisationType) {
            case AbstractSteppedModel::Step::Absolute:
                return stabilisationParamAbsolute.pascalsStr();
            case AbstractSteppedModel::Step::Relative:
                return QString::number(stabilisationParamRelative);
            }
            return "unknown_stabilisationParam";
        }
        QString timeOfCriterionTimeToStr() {
            return timeOfCriterionTime.toString();
        }
        QJsonObject toJson() {
            QJsonObject jobj;
            {
                jobj["criterionType"] = criterionToStr();
                jobj["stabilisationType"] = stabilisationTypeToStr();
                jobj["target"] = QString::number(cellPressure.pascals());//QString::number(getTarget());
                jobj["stabilisationParam"] = stabilisationParamToStr();
                jobj["timeOfCriterionTime"] = timeOfCriterionTimeToStr();
            }

            return jobj;
        }
    };
    QList<Step> m_steps;
    QString m_type;
    Measurements::Pressure m_overPressureCellVolumeter;
    Measurements::Pressure m_overPressurePoreVolumeter;
};



#endif // ABSTRACTSTEPPEDMODEL_H
