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
    ui->cmbData1->setVisible(false);
    Qt::GlobalColor color = Qt::darkRed;
//    for (int i = 0; i < ui->cmbData1->count(); ++i) {
    auto list = data->keys();
    for (int i = 0; i < list.size(); ++i) {
        ui->widget->addTrace(m_data->value(list[i]), tr("Время (t, мин)"), tr("%1, %2").arg(list[i]).arg("mm"), color, list[i]);
        color = static_cast<Qt::GlobalColor>(color + 1);
    }
    QwtLegend *legend = new QwtLegend();
    ui->widget->m_plot->insertLegend(legend, QwtPlot::RightLegend);

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

