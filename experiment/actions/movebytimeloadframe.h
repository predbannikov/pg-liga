#ifndef MOVEBYTIMELOADFRAME_H
#define MOVEBYTIMELOADFRAME_H

#include <QObject>

#include "baseaction.h"

class MoveByTimeLoadFrame : public BaseAction
{
    Q_OBJECT

public:
    explicit MoveByTimeLoadFrame(QObject *parent = nullptr);
    ~MoveByTimeLoadFrame();

    void init() override;
    bool update() override;
    void finishing() override;
    void pausing() override;

private:

    QTimer elapseTimeBase;
    enum TRANS {TRANS_1, TRANS_2} trans = TRANS_1;

};

#endif // MOVEBYTIMELOADFRAME_H
