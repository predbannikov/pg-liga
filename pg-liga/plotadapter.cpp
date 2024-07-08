#include <QDebug>

#include "plotadapter.h"

#include "experimentdata.h"

static constexpr auto SECONDS_IN_MINUTE = 1.0;     // TODO CHANGED 60

PlotAdapter::PlotAdapter(ExperimentData *model):
    m_model(model)
{}

QPointF PlotAdapter::sample(size_t idx) const
{
    

    const auto sample = m_model->point(idx);
    const auto beginTime = m_model->beginTime();
    const auto dt = beginTime.secsTo(sample.dateTime) / SECONDS_IN_MINUTE;

    

    return QPointF(dt, sample.value);
}

size_t PlotAdapter::size() const
{
    
    

    return m_model->pointCount();
}

QRectF PlotAdapter::boundingRect() const
{
    

    const auto beginTime = m_model->beginTime();
    const auto endTime = m_model->endTime();
    const auto dt = beginTime.secsTo(endTime) / SECONDS_IN_MINUTE;

    const auto bottomLeft = QPointF(0, m_model->minValue());
    const auto topRight = QPointF(dt, m_model->maxValue());

    

    return QRectF(bottomLeft, topRight);
}
