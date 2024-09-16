#ifndef STEPPEDPRESSURE_H
#define STEPPEDPRESSURE_H

#include <QObject>

#include "baseaction.h"

class SteppedPressure : public BaseAction
{
    Q_OBJECT

public:
    explicit SteppedPressure(QObject *parent = nullptr);
    ~SteppedPressure();

    void init() override;
    bool update() override;
    void finishing() override;
    void pausing() override;

private:

    QTimer elapseTime;
    enum TRANS {TRANS_1,    // Обнуление
                TRANS_2,    // Проверка обнуления
                TRANS_3,    // Задача целевого значения
                TRANS_4,
                TRANS_5,
                TRANS_6,
                TRANS_7,
               } trans = TRANS_1;

    int curStep = 0;
};

#endif // STEPPEDPRESSURE_H
