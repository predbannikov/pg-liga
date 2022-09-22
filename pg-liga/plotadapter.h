#ifndef PLOTADAPTER_H
#define PLOTADAPTER_H

#include <QPointF>
#include <qwt_series_data.h>

class ExperimentData;

class PlotAdapter : public QwtSeriesData<QPointF>
{
public:
    PlotAdapter(ExperimentData *model);

    QPointF sample(size_t idx) const override;
    size_t size() const override;
    QRectF boundingRect() const override;

private:
    ExperimentData *m_model;
};

#endif // PLOTADAPTER_H
