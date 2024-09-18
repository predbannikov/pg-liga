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
    void clear();
    ~CustomGraph();

private slots:
    void on_btnCreatGraph_clicked();

private:
    Ui::CustomGraph *ui;

    QMap<QString, ExperimentData *> *m_data;
};

#endif // CUSTOMGRAPH_H
