#include "decoratedplot.h"

#include "plotadapter.h"
#include "plotxyadapter.h"
#include "experimentdata.h"

#include <qwt_plot.h>
#include <qwt_text.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>


#include <QVBoxLayout>
#include <QPen>
#include <QTimer>
#include <QDebug>

DecoratedPlot::DecoratedPlot(QWidget *parent):
    QWidget(parent),
    m_plot(new QwtPlot(this))
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(9, 9, 9, 9);
    layout->addWidget(m_plot);

    zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, m_plot->canvas());
    zoomer->setRubberBand(QwtPicker::RectRubberBand);
    zoomer->setRubberBandPen(QColor(Qt::green));
    zoomer->setTrackerMode(QwtPicker::AlwaysOn);
    zoomer->setTrackerPen(QColor(Qt::black));

    panner = new QwtPlotPanner(m_plot->canvas());
    panner->setMouseButton(Qt::MiddleButton);


    auto *grid = new QwtPlotGrid();

    grid->setMajorPen(Qt::lightGray, 1.0);
    grid->setMinorPen(Qt::lightGray, 1.0, Qt::DotLine);
    grid->enableXMin(true);
    grid->enableYMin(true);
    grid->attach(m_plot);


    // Создаем кнопку для автоматического масштабирования
    QPushButton *autoScaleButton = new QPushButton("Автоматический масштаб", this);
    connect(autoScaleButton, &QPushButton::clicked, this, &DecoratedPlot::autoScale);
    layout->addWidget(autoScaleButton);

    autoScale();
}

void DecoratedPlot::addTrace(ExperimentData *data, const QString &ylabel)
{
    addTrace(data, tr("Время, мин"), ylabel);
}

void DecoratedPlot::addTrace(ExperimentData *data, const QString &xlabel, const QString &ylabel, Qt::GlobalColor color, QString label)
{
    setupLabels(xlabel, ylabel);

    auto *adapter = new PlotAdapter(data);
    auto *curve = new QwtPlotCurve(label);

    curve->setData(adapter);
    curve->setPen(color, 2.0);
//    curve->setStyle(QwtPlotCurve::Lines);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

//    curve->setCurveAttribute(QwtPlotCurve::Fitted);

    curve->attach(m_plot);

    connect(data, &ExperimentData::pointInserted, m_plot, &QwtPlot::replot);

    m_plot->replot();
}

void DecoratedPlot::addTrace(ExperimentData *xdata, ExperimentData *ydata, const QString &xlabel, const QString &ylabel)
{
    setupLabels(xlabel, ylabel);

    m_xdata.append(xdata);
    m_ydata.append(ydata);

    auto *adapter = new PlotXYAdapter(xdata, ydata);
    auto *curve = new QwtPlotCurve();

    curve->setPen(Qt::darkGreen, 2.0);
    curve->setData(adapter);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->attach(m_plot);

    connect(xdata, &ExperimentData::pointInserted, this, &DecoratedPlot::onNewDataAvailable);
    connect(ydata, &ExperimentData::pointInserted, this, &DecoratedPlot::onNewDataAvailable);

    m_plot->replot();
}

void DecoratedPlot::addAuxTrace(ExperimentData *xdata, ExperimentData *ydata, const QColor &color)
{
    m_xdata.append(xdata);
    m_ydata.append(ydata);

    auto *adapter = new PlotXYAdapter(xdata, ydata);
    auto *curve = new QwtPlotCurve();

    auto pen = curve->pen();
    pen.setColor(color);
    pen.setWidth(2);
    pen.setStyle(Qt::PenStyle::DashLine);

    curve->setPen(pen);
    curve->setData(adapter);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->attach(m_plot);

    connect(xdata, &ExperimentData::pointInserted, this, &DecoratedPlot::onNewDataAvailable);
    connect(ydata, &ExperimentData::pointInserted, this, &DecoratedPlot::onNewDataAvailable);

    m_plot->replot();
}

void DecoratedPlot::removeTrace(ExperimentData *data)
{
    disconnect(data, &ExperimentData::pointInserted, m_plot, &QwtPlot::replot);
}

void DecoratedPlot::removeTrace(ExperimentData *xdata, ExperimentData *ydata)
{
    disconnect(xdata, &ExperimentData::pointInserted, this, &DecoratedPlot::onNewDataAvailable);
    disconnect(ydata, &ExperimentData::pointInserted, this, &DecoratedPlot::onNewDataAvailable);
}

void DecoratedPlot::showText(const QString &text)
{
    m_plot->setTitle(text);
}

void DecoratedPlot::addLegends(QwtLegend *legend, QwtPlot::LegendPosition legendPosition)
{
    m_plot->insertLegend(legend, legendPosition);
}

void DecoratedPlot::onNewDataAvailable()
{
    auto equalPointCount = true;
    auto enoughPoints = false;

    /* m_xdata.size() is guaranteed to be equal to m_ydata.size() */
    for(auto i = 0; i < m_xdata.size(); ++i) {
        const auto pointCountX = m_xdata.at(i)->pointCount();
        const auto pointCountY = m_ydata.at(i)->pointCount();

        equalPointCount = equalPointCount && (pointCountX == pointCountY); /* Do not replot if uneven point count in any trace */
        enoughPoints = enoughPoints || ((pointCountX >= 2) && (pointCountY >= 2)); /* Replot if at least one trace with enough points */
    }

    if(enoughPoints && equalPointCount) {
        m_plot->replot();
    }
}

void DecoratedPlot::setupLabels (const QString &xlabel, const QString &ylabel)
{
    QwtText titleX(xlabel);
    QwtText titleY(ylabel);

    m_plot->setAxisTitle (QwtPlot::xBottom, titleX);
    m_plot->setAxisTitle (QwtPlot::yLeft  , titleY);
}
