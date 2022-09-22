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

static const int CYCLE_ACTIONS_TIMEINTERVAL_MSEC = 1000;

DecoratedPlot::DecoratedPlot(QWidget *parent):
    QWidget(parent),
    m_plot(new QwtPlot(this))
{
    

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(9, 9, 9, 9);
    layout->addWidget(m_plot);

    auto *grid = new QwtPlotGrid();

    grid->setMajorPen(Qt::lightGray, 1.0);
    grid->setMinorPen(Qt::lightGray, 1.0, Qt::DotLine);
    grid->enableXMin(true);
    grid->enableYMin(true);
    grid->attach(m_plot);

/* !!!ATT!!! Временно убрано 04.04.2022 г.
   в процессе поиска причин вылета программы

// !!!ATT!!! проверка возможности менять диапазон шкалы на графике
//    m_plot->setAxisAutoScale (QwtPlot::yLeft, false);
//    m_plot->setAxisScale (QwtPlot::yLeft, 0.0, 1550.0);
    // !!!ATT!!! - временное добавление 28.02.2022 г.
//    m_TestButton = new QPushButton (this);
//    layout->addWidget (m_TestButton);
    // значения по умолчанию для границ при выбранном ручном масштабе
    m_AxisRangeXmin =    0.0;
    m_AxisRangeXmax = 1000.0;
    m_AxisRangeYmin =    0.0;
    m_AxisRangeYmax = 1000.0;
    // создаем экземпляр класса QwtChartZoom, и, если необходимо, переопределяем цвет каймы выделяемой области, задающей новый масштаб графика
    zoom = new QwtChartZoom (m_plot);
    zoom->setRubberBandColor (Qt::white);
    // создаем экземпляр класса QWheelZoomSvc, и прикрепляем его к менеджеру масштабирования
    whlzmsvc = new QWheelZoomSvc ();
    whlzmsvc->attach (zoom);
    // создаем экземпляр класса QAxisZoomSvc, и прикрепляем его к менеджеру масштабирования
    axzmsvc = new QAxisZoomSvc ();
    axzmsvc->attach (zoom);

    // Hints for Axis Scale Instruments
    showScaleHints ();

    // таймер вызова циклических действий (используется, в частности, для
    // выдерживания паузы перед восстановлением автомасштабирования)
    QTimer  *cycle_timer = new QTimer (this);
    connect (cycle_timer, SIGNAL(timeout()), this, SLOT(CycleActions()));
    cycle_timer->start (CYCLE_ACTIONS_TIMEINTERVAL_MSEC);
*/
    
}

void DecoratedPlot::addTrace(ExperimentData *data, const QString &ylabel)
{
    

    addTrace(data, tr("Время, мин"), ylabel);

    
}

void DecoratedPlot::addTrace(ExperimentData *data, const QString &xlabel, const QString &ylabel)
{
    

    setupLabels(xlabel, ylabel);

    auto *adapter = new PlotAdapter(data);
    auto *curve = new QwtPlotCurve();

    curve->setData(adapter);
    curve->setPen(Qt::darkGreen, 2.0);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

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

//    auto font = this->font();

//    font.setBold(true);
//    font.setItalic(true);

//    titleX.setFont(font);
//    titleY.setFont(font);

    m_plot->setAxisTitle (QwtPlot::xBottom, titleX);
    m_plot->setAxisTitle (QwtPlot::yLeft  , titleY);

    
}

void DecoratedPlot :: setAxisRangeX_Auto ()
{
    

    m_plot->setAxisAutoScale (QwtPlot::xBottom, true );
    m_plot->replot ();

    
}
void DecoratedPlot :: setAxisRangeX_Man  ()
{
    

    m_plot->setAxisScale (QwtPlot::xBottom, m_AxisRangeXmin, m_AxisRangeXmax);
    m_plot->setAxisAutoScale (QwtPlot::xBottom, false);
    m_plot->replot ();

    
}
void DecoratedPlot :: setAxisRangeY_Auto ()
{
    

    m_plot->setAxisAutoScale (QwtPlot::yLeft  , true );
    m_plot->replot ();

    
}
void DecoratedPlot :: setAxisRangeY_Man  ()
{
    

    m_plot->setAxisScale (QwtPlot::yLeft  , m_AxisRangeYmin, m_AxisRangeYmax);
    m_plot->setAxisAutoScale (QwtPlot::yLeft  , false);
    m_plot->replot ();

    
}
bool DecoratedPlot :: axisRangeX_isAuto ()
{
    

    

    return m_plot->axisAutoScale (QwtPlot::xBottom);
}
bool DecoratedPlot :: axisRangeY_isAuto ()
{
    

    

    return m_plot->axisAutoScale (QwtPlot::yLeft  );
}


// показ картинок со стрелочками - подсказок о возможности управления масштабом графика
void DecoratedPlot :: showScaleHints ()
{
    

//  QPixmap hint_pic = QPixmap (":/images/ScaleHintVertical_01m.png");
    QPixmap hint_pic = QPixmap (":/images/ScaleHintVertical_02m.png");
    QRect   hint_pic_geometry = hint_pic.rect();

    // Если элементы, показывающие подсказки для масштабирования ещё не созданы,
    // они создаются
    if (m_ScaleHintYTop == nullptr)
    {
//      m_ScaleHintYTop = new QPushButton (m_plot);
//      m_ScaleHintYTop->setText("↕");
//      m_ScaleHintYTop = new QLabel (m_plot);
        m_ScaleHintYTop = new QLabel (this);
//      m_ScaleHintYTop->setText("↕");
        m_ScaleHintYTop->setPixmap (hint_pic);
    }
/*
    if (m_ScaleHintYBottom == nullptr)
    {
//      m_ScaleHintYBottom = new QPushButton (m_plot);
//      m_ScaleHintYBottom->setText("↕");
//      m_ScaleHintYBottom = new QLabel (m_plot);
        m_ScaleHintYBottom = new QLabel (this);
//      m_ScaleHintYBottom->setText("↕");
        m_ScaleHintYBottom->setPixmap (hint_pic);
    }
*/
    int x, y, h, w;
//  QRect frame = m_plot->geometry();
    QRect frame = geometry();
    x = frame.x() + 5;
    y = frame.y() + 5;
    h = hint_pic_geometry.height() + 5;
    w = hint_pic_geometry.width () + 1;
    m_ScaleHintYTop   ->setGeometry (x, y, w, h);
//  y = frame.y() + height() - h - 5;
//  m_ScaleHintYBottom->setGeometry (x, y, w, h);

    // вывод подсказок на самый верх
    m_plot->stackUnder (m_ScaleHintYTop   );
//  m_plot->stackUnder (m_ScaleHintYBottom);

    
}
void DecoratedPlot :: CycleActions ()
{
//  

    // Проверка наступления необходимости восстановления автомасштабирования графика
    bool was_greater_than_zero = (m_MsecToRestoreAxisAutoscale > 0);
    m_MsecToRestoreAxisAutoscale -= CYCLE_ACTIONS_TIMEINTERVAL_MSEC;
    if ((was_greater_than_zero) && (m_MsecToRestoreAxisAutoscale <= 0))
    {
        m_MsecToRestoreAxisAutoscale = 0;
        // восстановление автомасштабирования графика
        RestoreAxisAutoscale ();
    }

//  
}
// восстановление автомасштабирования графика
void DecoratedPlot :: RestoreAxisAutoscale ()
{
    

    // восстанавливаем автомасштабирование только по оси X,
    // так как если это ось времени, то её шкала должна
    // постоянно меняться
    if (!axisRangeX_isAuto ()) setAxisRangeX_Auto ();

    
}
