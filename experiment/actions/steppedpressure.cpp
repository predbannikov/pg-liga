#include "steppedpressure.h"

#include <QDebug>
#include <algorithm>
#include <numeric>

#include "volumeter1.h"

SteppedPressure::SteppedPressure(QObject *parent)
    : ActionCycle{parent}
{

}

SteppedPressure::~SteppedPressure()
{
    qDebug() << Q_FUNC_INFO;
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

bool SteppedPressure::updateSteping()
{
    // Тут начать задавать логику ступени
    switch(trans) {

    case TRANS_1:
        jCmdToQueue["CMD"] = "volumetr1_set_target";
        jCmdToQueue["target"] = jStep["target"].toString();
        putQueue(jCmdToQueue);
        elapseTime.start(1000);
        store->startStep(jStep);
        trans = TRANS_2;
        break;

    case TRANS_2:
        if (volumeter1->pressureSens->value > (jStep["target"].toString().toDouble() - 3000)) {
            if (jStep["criterionType"].toString() == "Stabilisation") {
                trans = TRANS_3;
            }
            else if (jStep["criterionType"].toString() == "Duration")
                trans = TRANS_4;
        }
        break;

    case SteppedPressure::TRANS_3: {
        Measurements::TimeLongInterval time = Measurements::TimeLongInterval::fromStringLong(jStep["timeOfCriterionTime"].toString());
        if (store->data["CellPressure_kPa"]->valueFromBack(jStep["time_start_step"].toString().toInt(), time.milliseconds()).first) {
            qDebug() << "time out, stabilisation needed";
        }
        qDebug() << "------";
        break;
    }

    case SteppedPressure::TRANS_4:
        break;
    case SteppedPressure::TRANS_5:
        break;
    case SteppedPressure::TRANS_6:
        break;

    case SteppedPressure::TRANS_7:


        break;
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
