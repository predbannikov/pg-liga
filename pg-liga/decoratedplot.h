#ifndef DECORATEDPLOT_H
#define DECORATEDPLOT_H

#include <QWidget>
#include <QDebug>

class QwtPlot;
class ExperimentData;

// !!!ATT!!! - временное добавление 28.02.2022 г.
#include <QPushButton>
#include <QLabel>
#include "qwtchartzoom.h"
#include "qwheelzoomsvc.h"
#include "qaxiszoomsvc.h"
// пауза (в мсек) перед восстановлением автоматического масштабирования
// после завершения пользователем действий с графиком
const long int PAUSE_BEFORE_RESTORE_AXIS_AUTOSCALE = 30000;

class DecoratedPlot : public QWidget
{
    Q_OBJECT

public:
    DecoratedPlot(QWidget *parent = nullptr);

/* !!!ATT!!! Временно убрано 04.04.2022 г.
   в процессе поиска причин вылета программы

   ~DecoratedPlot ()
    {
        delete axzmsvc ;
        delete whlzmsvc;
        delete zoom;
    }
*/
    void addTrace    (ExperimentData *data , const QString &ylabel);
    void addTrace    (ExperimentData *data , const QString &xlabel, const QString &ylabel);
    void addTrace    (ExperimentData *xdata, ExperimentData *ydata, const QString &xlabel, const QString &ylabel);
    void addAuxTrace (ExperimentData *xdata, ExperimentData *ydata, const QColor &color);

    void removeTrace(ExperimentData* data);
    void removeTrace(ExperimentData* xdata, ExperimentData *ydata);

    void showText(const QString &text);

    // !!!ATT!!! - временное добавление 28.02.2022 г.
    void setAxisRangeX_Auto ();
    void setAxisRangeX_Man  ();
    void setAxisRangeY_Auto ();
    void setAxisRangeY_Man  ();
    bool axisRangeX_isAuto ();
    bool axisRangeY_isAuto ();
    void setManAxisRangesForX (double a_Xmin, double a_Xmax) {m_AxisRangeXmin = a_Xmin; m_AxisRangeXmax = a_Xmax;}
    void setManAxisRangesForY (double a_Ymin, double a_Ymax) {m_AxisRangeYmin = a_Ymin; m_AxisRangeYmax = a_Ymax;}

/* !!!ATT!!! Временно убрано 04.04.2022 г.
   в процессе поиска причин вылета программы

    virtual void resizeEvent ( QResizeEvent * e )
    {
        

        QWidget::resizeEvent (e);
        showScaleHints ();
    }
    virtual void paintEvent ( QPaintEvent * e )
    {
        

        QWidget::paintEvent (e);
        showScaleHints ();
    }
    virtual bool event (QEvent* event)
    {
        

        // смысл обработки событий здесь такой:
        // пока пользователь что-то делает с графиком (меняет масштаб с помощью мышки),
        // ему никто не мешает, но когда он прекращает действия, после паузы включается
        // автомасштабирование горизонтальной оси. Соответственно, здесь производится
        // сброс таймера паузы при наличии действий пользователя
        bool user_works_with_plot = false;
        if (event->type() == QEvent::MouseButtonPress  ) user_works_with_plot = true;
        if (event->type() == QEvent::MouseButtonRelease) user_works_with_plot = true;
        if (event->type() == QEvent::MouseMove         )
        {
            QMouseEvent* me = (QMouseEvent*) event;
            if (me->buttons()                          ) user_works_with_plot = true;
        }
        if (event->type() == QEvent::Wheel             ) user_works_with_plot = true;

        // установка паузы
        if (user_works_with_plot) m_MsecToRestoreAxisAutoscale = PAUSE_BEFORE_RESTORE_AXIS_AUTOSCALE;

        

        return false;
    }
*/
private slots:
    void onNewDataAvailable();
    void CycleActions ();
private:
    void setupLabels(const QString &xlabel, const QString &ylabel);

    QwtPlot *m_plot;

    // !!!ATT!!! - временное добавление 28.02.2022 г.
    QPushButton *m_TestButton;
    double m_AxisRangeXmin;
    double m_AxisRangeXmax;
    double m_AxisRangeYmin;
    double m_AxisRangeYmax;
    QwtChartZoom  *zoom;
    QWheelZoomSvc *whlzmsvc;
    QAxisZoomSvc  *axzmsvc;

    void showScaleHints ();
//  QPushButton *m_ScaleHintYTop    = nullptr;
//  QPushButton *m_ScaleHintYBottom = nullptr;
    QLabel *m_ScaleHintYTop    = nullptr;
    QLabel *m_ScaleHintYBottom = nullptr;

    long int m_MsecToRestoreAxisAutoscale = PAUSE_BEFORE_RESTORE_AXIS_AUTOSCALE;
    // восстановление автомасштабирования графика
    void RestoreAxisAutoscale ();

    QVector<ExperimentData*> m_xdata;
    QVector<ExperimentData*> m_ydata;
};

#endif // DECORATEDPLOT_H
