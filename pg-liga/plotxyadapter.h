#ifndef PLOTXYADAPTER_H
#define PLOTXYADAPTER_H

#include <QPointF>
#include <qwt_series_data.h>

class ExperimentData;

class PlotXYAdapter : public QwtSeriesData<QPointF>
{
public:
    PlotXYAdapter(ExperimentData *xmodel, ExperimentData *ymodel);

    QPointF sample(size_t idx) const override;
    size_t size() const override;
    QRectF boundingRect() const override;

private:
    ExperimentData *m_xmodel;
    ExperimentData *m_ymodel;
};

#endif // PLOTXYADAPTER_H
