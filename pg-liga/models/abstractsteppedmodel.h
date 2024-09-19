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
        enum LoadMeasurement {
            Pressure,
            Newtones
        };



       //Steppedloadingmodel
//        LoadMeasurement load_loadMeasurement;
//        Measurements::Force load_force;
//        Measurements::Length load_stabilisationParamAbsolute;
//        Measurements::TimeLongInterval durationTime;
//        Measurements::TimeLongInterval stabilisationTime;


       //SteppedPressuriseModel

          Measurements::Pressure cellPressure; //target ->
          Measurements::Pressure stabilisationParamAbsolute; //-какой параметр
          Measurements::TimeInterval timeOfCriterionTime; //Время
          CriterionType criterion; //Критерий (*, вермя, стабилизация)
          StabilisationType stabilisationType;//Тип стабилизации;
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
                return stabilisationParamAbsolute.kiloPascalsStr();
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
                jobj["target"] = cellPressure.pascals()/1000;//QString::number(getTarget());
                jobj["stabilisationParam"] = stabilisationParamToStr();
                jobj["timeOfCriterionTime"] = timeOfCriterionTimeToStr();
//                jobj["force"] = QString::number(load_force.kiloNewtons());
//                jobj["mesurement"] = MeasurementToString();
//                jobj["stabilisationParamAbsoluteMilimetres"] = load_stabilisationParamAbsolute.millimetres();
//                jobj["durationTime"] = durationTime.toString();
//                jobj["stabilisationTime"] = stabilisationTime.toString();

            }

            return jobj;
        }
    };
    QList<Step> m_steps;
    QString m_type;
};



#endif // ABSTRACTSTEPPEDMODEL_H
