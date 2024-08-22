#include "customgraph.h"
#include "ui_customgraph.h"

CustomGraph::CustomGraph(QMap<QString, ExperimentData *> *data, QWidget *parent) :
    QWidget(parent),
    m_data(data),
    ui(new Ui::CustomGraph)
{
    ui->setupUi(this);
//    ui->widget = new DecoratedPlot(this);
    ui->cmbData1->addItems(data->keys());
    Qt::GlobalColor color = Qt::darkGreen;
    for (int i = 0; i < ui->cmbData1->count(); ++i) {
        ui->widget->addTrace(m_data->value(ui->cmbData1->itemText(i)), tr("Время (t, мин)"), tr("%1, %2").arg(ui->cmbData1->itemText(i)).arg("mm"), color);
        color = static_cast<Qt::GlobalColor>(color + 2);
    }

}

CustomGraph::~CustomGraph()
{
    delete ui;
}

void CustomGraph::on_btnCreatGraph_clicked()
{
//    ui->widget->addTrace(m_data->value(ui->comboBox->currentText()), tr("Время (t, мин)"), tr("%1, %2").arg(ui->comboBox->currentText()).arg("mm"));
    ui->widget->repaint();
}

