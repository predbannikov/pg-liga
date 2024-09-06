#ifndef ADJUSTLOADFRAME_H
#define ADJUSTLOADFRAME_H

#include "baseaction.h"

class AdjustByForceLoadFrame : public BaseAction
{
    Q_OBJECT
public:
    explicit AdjustByForceLoadFrame(QObject *parent = nullptr);
    ~AdjustByForceLoadFrame();

    void init() override;
    bool update() override;
    void finishing() override;

private:

    QTimer elapseTime;
    enum TRANS {TRANS_1, TRANS_2} trans = TRANS_1;
};

#endif // ADJUSTLOADFRAME_H
