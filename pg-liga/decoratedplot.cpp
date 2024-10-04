#include "decoratedplot.h"

#include "plotadapter.h"
#include "plotxyadapter.h"
#include "experimentdata.h"


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
    panner->setMouseButton(Qt::RightButton);
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


void DecoratedPlot::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        setCursor(Qt::ClosedHandCursor);  // Изменяем курсор на "руку" при захвате
        lastMousePosition = event->pos(); // Сохраняем начальную позицию мыши
    }
}

void DecoratedPlot::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::RightButton) {
        // Вычисляем смещение по осям относительно предыдущей позиции
        double dx = event->pos().x() - lastMousePosition.x();
        double dy = event->pos().y() - lastMousePosition.y();

        // Получаем текущие диапазоны осей
        double xLower = m_plot->axisScaleDiv(QwtPlot::xBottom).lowerBound();
        double xUpper = m_plot->axisScaleDiv(QwtPlot::xBottom).upperBound();
        double yLower = m_plot->axisScaleDiv(QwtPlot::yLeft).lowerBound();
        double yUpper = m_plot->axisScaleDiv(QwtPlot::yLeft).upperBound();

        // Рассчитываем коэффициенты для более плавного перемещения
        double xFactor = (xUpper - xLower) / m_plot->canvas()->width();
        double yFactor = (yUpper - yLower) / m_plot->canvas()->height();

        // Применяем смещение к диапазонам осей
//        m_plot->setAxisScale(QwtPlot::xBottom, xLower - dx * xFactor, xUpper - dx * xFactor);
//        m_plot->setAxisScale(QwtPlot::yLeft, yLower + dy * yFactor, yUpper + dy * yFactor);
//        m_plot->replot(); // Перерисовываем график
//        lastMousePosition = event->pos();  // Обновляем позицию мыши
        // Применяем смещение к диапазонам осей
        double newXLower = xLower + dx * xFactor;
        double newXUpper = xUpper + dx * xFactor;
        double newYLower = yLower - dy * yFactor;
        double newYUpper = yUpper - dy * yFactor;
        // Обновляем оси
        m_plot->setAxisScale(QwtPlot::xBottom, newXLower, newXUpper);
        m_plot->setAxisScale(QwtPlot::yLeft, newYLower, newYUpper);
        // Перерисовываем график
        m_plot->replot();

        // Обновляем последнюю позицию мыши
        lastMousePosition = event->pos();
    }
}

void DecoratedPlot::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        setCursor(Qt::ArrowCursor);  // Возвращаем стандартный курсор
    }
}
