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

    bool updateSteping() override;
    bool stepChanged() override;


private:
    bool betaLeastSquares(int n);


    QTimer elapseTime;
    enum TRANS {TRANS_1,    // Обнуление
                TRANS_2,    // Проверка обнуления
                TRANS_3,    // Задача целевого значения
                TRANS_4,
                TRANS_5,
                TRANS_6,
                TRANS_7,
                TRANS_8,
               } trans = TRANS_1;

    int curStep = 0;
};

#endif // STEPPEDPRESSURE_H
