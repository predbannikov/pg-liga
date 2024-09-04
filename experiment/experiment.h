#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "operations.h"


class Experiment : public Operations
{

public:
    Experiment(quint8 addr);
    ~Experiment();
    void experimentParser() override;

public slots:
    void doWork();


private:

};

#endif // EXPERIMENT_H
