#ifndef STEPPEDPRESSURE_H
#define STEPPEDPRESSURE_H

#include <QObject>

#include "baseaction.h"
#include "actioncycle.h"

class SteppedPressure : public ActionCycle
{
    Q_OBJECT

public:
    explicit SteppedPressure(QObject *parent = nullptr);
    ~SteppedPressure();

    bool initStepping() override;
    bool updateSteping() override;
    bool stepChanged() override;


private:
    bool betaLeastSquares(int n);


    enum TRANS {
        TRANS_ENABLE_CTRL,
        TRANS_SET_TARGET,            // Набор целевого значения
        TRANS_TIMEWAIT,
        TRANS_DURATION,
        TRANS_STABILISATION_CRITERION_MET,
        TRANS_6,
        TRANS_FINISH_STEP,
        TRANS_8,
    } trans = TRANS_ENABLE_CTRL;

    Measurements::TimeLongInterval criterionTime;
};

#endif // STEPPEDPRESSURE_H
