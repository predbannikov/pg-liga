#include <QDebug>

#include "plotxyadapter.h"
#include "experimentdata.h"

PlotXYAdapter::PlotXYAdapter(ExperimentData *xmodel, ExperimentData *ymodel):
    m_xmodel(xmodel),
    m_ymodel(ymodel)
{}

QPointF PlotXYAdapter::sample(size_t idx) const
{
    

    const auto xsample = m_xmodel->point(idx);
    const auto ysample = m_ymodel->point(idx);

    

    return QPointF(xsample.value, ysample.value);
}

size_t PlotXYAdapter::size() const
{
    

    const auto xsize = m_xmodel->pointCount();
    const auto ysize = m_ymodel->pointCount();

    if(xsize == ysize) {
        
        return xsize;
    } else {
        
        return 0;
    }

    
}

QRectF PlotXYAdapter::boundingRect() const
{
    

    if(size()) {
        const auto xmin = m_xmodel->minValue();
        const auto ymin = m_ymodel->minValue();

        const auto xmax = m_xmodel->maxValue();
        const auto ymax = m_ymodel->maxValue();

        const auto bottomLeft = QPointF(xmin, ymin);
        const auto topRight = QPointF(xmax, ymax);

        

        return QRectF(bottomLeft, topRight);

    } else {
        
        return QRectF();
    }

    
}
