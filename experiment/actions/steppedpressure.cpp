#include "steppedpressure.h"

#include <QDebug>
#include <algorithm>
#include <numeric>

#include "volumeter1.h"
#include "volumeter2.h"

SteppedPressure::SteppedPressure(QObject *parent)
    : ActionCycle{parent}
{

}

SteppedPressure::~SteppedPressure()
{
    qDebug() << Q_FUNC_INFO;
}

bool SteppedPressure::initStepping()
{
    trans = TRANS_ENABLE_CTRL;
}

bool SteppedPressure::stepChanged()
{
    return false;
}

//void SteppedPressure::init() {
//    elapseTime.setSingleShot(true);
//    elapseTime.setInterval(1000);
//    curStep = jAction["curStep"].toString().toInt();
//}

#define TOLERANCE   3000

bool SteppedPressure::updateSteping()
{
    // Тут начать задавать логику ступени
    switch(trans) {

    case TRANS_ENABLE_CTRL:
        jCmdToQueue["CMD"] = "volumetr1_set_target";
        jCmdToQueue["target"] = jStep["target"].toString();
        putQueue(jCmdToQueue);
        trans = TRANS_SET_TARGET;
        break;

    case TRANS_SET_TARGET:
        if (volumeter1->pressureSens->value > (jStep["target"].toString().toDouble() - TOLERANCE)
                && volumeter1->pressureSens->value < (jStep["target"].toString().toDouble() + TOLERANCE)) {
            trans = TRANS_TIMEWAIT;
            criterionTime = Measurements::TimeLongInterval::fromStringLong(jStep["timeOfCriterionTime"].toString());
            store->startStep(jStep);
        }
        break;

    case SteppedPressure::TRANS_TIMEWAIT: {
        if (store->data["PorePressure_kPa"]->valueFromBackOfStepTime(jStep["time_start_step"].toString().toInt(), criterionTime.milliseconds()).first) {
            qDebug() << "time out, stabilisation needed";
            if (jStep["criterionType"].toString() == "Stabilisation") {
                trans = TRANS_STABILISATION_CRITERION_MET;
            }
            else if (jStep["criterionType"].toString() == "DurationAfterSetup") {
                trans = TRANS_DURATION;
            }
        }
        qDebug() << "------";
        break;
    }

    case SteppedPressure::TRANS_DURATION:
        break;
    case SteppedPressure::TRANS_STABILISATION_CRITERION_MET:
    {
        const auto currentPoint = volumeter2->pressureSens->value;
        const auto prevPoint = store->data["PorePressure_kPa"]->valueFromBackOfStepTime(jStep["time_start_step"].toString().toInt(), criterionTime.milliseconds()).second;
        double delta = 0;
        if(jStep["stabilisationType"].toString() == "Absolute") {
           delta = fabs(currentPoint - prevPoint);

        } else if(jStep["stabilisationType"].toString() == "Relative") {
            delta = fabs(currentPoint - prevPoint) / currentPoint;
        }
        qDebug() << QString("currentPoint=%1    prevPoint=%2    delta=%3    param=%4").arg(currentPoint).arg(prevPoint).arg(delta)
                    .arg(jStep["stabilisationParam"].toString().toDouble());
        if (delta <= jStep["stabilisationParam"].toString().toDouble())
            trans = TRANS_FINISH_STEP;
        return false;
    }

    case SteppedPressure::TRANS_6:
        break;

    case SteppedPressure::TRANS_FINISH_STEP:
        store->stopStep(jStep);
        return true;
    case SteppedPressure::TRANS_8:
        qDebug() << "time needed";
        break;
    }
    return false;
}

//void SteppedPressure::finishing()
//{
//}

//void SteppedPressure::pausing()
//{
//}

bool SteppedPressure::betaLeastSquares(int n)
{
    auto stepDataCellPressure = volumeter1->store->data["CellPressure_kPa"]->getDataOfStartTime();
    auto stepDataPorePressure = volumeter1->store->data["PorePressure_kPa"]->getDataOfStartTime();

    if (stepDataCellPressure.size() < n || stepDataPorePressure.size() < n)
        return false;
    QVector<double> xs;
    for (int i = stepDataCellPressure.size() - n; i < stepDataCellPressure.size(); ++i)
        xs.append(stepDataCellPressure[i].second);
    QVector<double> ys;
    for (int i = stepDataPorePressure.size() - n; i < stepDataPorePressure.size(); ++i)
        ys.append(stepDataPorePressure[i].second);

    QVector<double> xys;
    std::transform(xs.cbegin(), xs.cend(), ys.cbegin(), std::back_inserter(xys), std::multiplies<double>());

    const auto xSum = std::accumulate (xs.cbegin(), xs.cend(), 0.0);
    const auto ySum = std::accumulate (ys.cbegin(), ys.cend(), 0.0);
    const auto xySum = std::accumulate (xys.cbegin(), xys.cend(), 0.0);
    const auto xSumSq = std::accumulate (xs.cbegin(), xs.cend(), 0.0, [](double result,double m) {
        return result + m * m;
    });

    const double slope = (n * xySum - xSum * ySum) / (n * xSumSq - xSum * xSum);
    const double intercept = (ySum - slope * xSum) / n;

    const auto x1 = 0.1;
    const auto x2 = xs.last();
    const auto y1 = intercept;
    const auto y2 = slope * x2 + y1;
    const auto B = (y2 - y1) / (x2 - x1);

    qDebug() << "B=" << B;
    if (B > 0.95)
        return true;
    return false;
}
