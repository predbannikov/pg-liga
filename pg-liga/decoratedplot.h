#ifndef DECORATEDPLOT_H
#define DECORATEDPLOT_H

#include <QWidget>
#include <QDebug>

class QwtPlot;
class QwtPlotPanner;
class QwtPlotZoomer;
class ExperimentData;

#include <QPushButton>
#include <QLabel>
#include "qwtchartzoom.h"
//#include "qwheelzoomsvc.h"
//#include "qaxiszoomsvc.h"
#include <qwt_plot_panner.h>
#include <qwt_plot_zoomer.h>

class DecoratedPlot : public QWidget
{
    Q_OBJECT

public:
    DecoratedPlot(QWidget *parent = nullptr);

    void addTrace    (ExperimentData *data , const QString &ylabel);
    void addTrace    (ExperimentData *data , const QString &xlabel, const QString &ylabel, Qt::GlobalColor color = Qt::GlobalColor::darkGreen);
    void addTrace    (ExperimentData *xdata, ExperimentData *ydata, const QString &xlabel, const QString &ylabel);
    void addAuxTrace (ExperimentData *xdata, ExperimentData *ydata, const QColor &color);

    void removeTrace(ExperimentData* data);
    void removeTrace(ExperimentData* xdata, ExperimentData *ydata);

    void showText(const QString &text);


    QwtPlot *m_plot;
private slots:
    void onNewDataAvailable();

    void autoScale() {
        // Автоматически масштабируем график для отображения всех данных
        m_plot->setAxisAutoScale(QwtPlot::xBottom);
        m_plot->setAxisAutoScale(QwtPlot::yLeft);
        m_plot->replot();

        // Сбрасываем масштаб и прокрутку
        zoomer->setZoomBase();
        panner->setEnabled(false);
        panner->setEnabled(true);
    }
private:
    void setupLabels(const QString &xlabel, const QString &ylabel);

    double m_AxisRangeXmin;
    double m_AxisRangeXmax;
    double m_AxisRangeYmin;
    double m_AxisRangeYmax;


    QVector<ExperimentData*> m_xdata;
    QVector<ExperimentData*> m_ydata;

    QwtPlotZoomer *zoomer = nullptr;
    QwtPlotPanner *panner = nullptr;

};

#endif // DECORATEDPLOT_H
