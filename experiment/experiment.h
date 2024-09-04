#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "operations.h"


class Experiment : public Operations
{

public:
    Experiment(quint8 addr);
    ~Experiment();
    void parser() override;

public slots:
    void doWork();


private:
    void experimentParser();

};

#endif // EXPERIMENT_H
