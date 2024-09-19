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

    case TRANS_1: {

        QJsonObject::iterator iter;
        for (iter = jAction.begin(); iter != jAction.end(); ++iter) {
            if (iter.key().contains("step") && iter.key().split('_')[1].toInt() == curStep) {
                qDebug() << "key=" << iter.key().split('_')[1] << "    curStep=" << curStep;
                trans = TRANS_2;
                return false;
            }
        }

    }
        break;

    case TRANS_2:
        jCmdToQueue["CMD"] = "volumetr1_stepper_set_zero";
        putQueue(jCmdToQueue);
        elapseTime.start(2000);
        trans = TRANS_3;
        break;

    case SteppedPressure::TRANS_3:
        if (!elapseTime.isActive() && volumeter1->stepper->position != 0) {
            sendError("Задержка больше 1000мс, по комманде volumetr1_stepper_set_zero", jAction);
            trans = TRANS_2;
        }
        else if (elapseTime.isActive() && volumeter1->stepper->position == 0) {
            trans = TRANS_4;
        }
        break;

    case SteppedPressure::TRANS_4:
        jCmdToQueue["CMD"] = "volumetr1_set_target";
        jCmdToQueue["target"] = jAction[QString("step_%1").arg(curStep)].toObject()["target"].toString();
        putQueue(jCmdToQueue);
        elapseTime.start(1000);
        trans = TRANS_6;
        break;

    case SteppedPressure::TRANS_5:
        // if (!elapseTime.isActive() && volumeter1->controller->status == 0) {
        //     sendError("Задержка больше 1000мс, по комманде volumetr1_set_target", jAction);
        //     trans = TRANS_4;
        // } else if (elapseTime.isActive() && volumeter1->controller->status != 0) {
        //     trans = TRANS_6;
        // }
        // break;

    case SteppedPressure::TRANS_6: {
        if (volumeter1->pressureSens->value > (jAction[QString("step_%1").arg(curStep)].toObject()["target"].toString().toDouble() - 3000)) {
            if (jAction[QString("step_%1").arg(curStep)].toObject()["criterionType"].toString() == "Stabilisation") {

                trans = TRANS_7;
            }
            else if (jAction[QString("step_%1").arg(curStep)].toObject()["criterionType"].toString() == "Duration")
                trans = TRANS_8;
        }
        // betaLeastSquares(3);
        // auto data = volumeter1->store->data["CellPressure_kPa"];
        // qint64 start_time, time;
        // data.getLastStartAndCurTime(start_time, time);
        // qDebug() << data.size() << start_time << time << data.valueFromBack(start_time, time) <<" ";

    }
        break;

    case SteppedPressure::TRANS_7:
        qDebug() << "stabilization needed";


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
