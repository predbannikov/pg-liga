#ifndef CUSTOMGRAPH_H
#define CUSTOMGRAPH_H

#include <QWidget>


#include "plotadapter.h"
#include "experimentdata.h"
#include "decoratedplot.h"

namespace Ui {
class CustomGraph;
}

class CustomGraph : public QWidget
{
    Q_OBJECT

public:
    explicit CustomGraph(QMap<QString, ExperimentData *> *data, QWidget *parent = nullptr);
    ~CustomGraph();

private slots:
    void on_btnCreatGraph_clicked();

private:
    Ui::CustomGraph *ui;

    QMap<QString, ExperimentData *> *m_data;
    DecoratedPlot *positionVsTime = nullptr;
};

#endif // CUSTOMGRAPH_H
