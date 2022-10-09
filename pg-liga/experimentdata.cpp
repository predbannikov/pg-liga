#include <QDebug>

#include "experimentdata.h"

ExperimentData::ExperimentData(QObject *parent):
    QObject(parent)
{
    
    
}

void ExperimentData::append(double value, const QDateTime &dateTime)
{
    

    m_data.append({value + m_offset, dateTime});
    emit pointInserted();

    
}

void ExperimentData::append(QList<QPair<qint64, float> > &list)
{
    for (auto &pair: list) {
        m_data.append({pair.second + m_offset, QDateTime::fromSecsSinceEpoch(pair.first / 1000)});
    }
    emit pointInserted();
}

ExperimentData::Point ExperimentData::pointBackInTime(Measurements::TimeInterval time) const
{
    

    if(pointCount() > 0) {
        if(time.siValue() == 0) {
            
            return pointLast();

        } else {
            const auto target = pointLast().dateTime.addSecs(static_cast<qint64>(-time.seconds()));

            for(auto i = pointCount() - 1; i >=0; --i) {
                const auto p = point(i);
                if(p.dateTime <= target) {
                    
                    return p;
                }
            }
        }

    }

    

    return Point();
}

void ExperimentData::setPoint(const ExperimentData::Point &point, int idx)
{
    

    if(idx >= pointCount()) {
        
        return;
    } else {
        m_data.replace(idx, point);
        emit pointInserted();
    }

    
}

ExperimentData::operator QVector<double>() const
{
    

    QVector<double> result;
    result.reserve(m_data.size());

    std::transform(m_data.cbegin(), m_data.cend(), std::back_inserter(result), [](const Point &pt) { return pt.value; });

    

    return result;
}

double ExperimentData::minValue() const
{
    

    if(m_data.isEmpty()) {
        
        return 0;

    } else {
        auto min = std::numeric_limits<double>::max();

        for(const auto &point: m_data) {
            auto value = point.value;

            if(value < min) {
                min = value;
            }
        }
        
        return min;
    }

    
}

double ExperimentData::maxValue() const
{
    

    if(m_data.isEmpty()) {
        
        return 0;

    } else {
        auto max = std::numeric_limits<double>::lowest();

        for(const auto &point: m_data) {
            auto value = point.value;

            if(value > max) {
                max = value;
            }
        }
        
        return max;
    }

    
}

double ExperimentData::lastDelta() const
{
    

    if(m_data.size() > 1) {
        
        return pointLast().value - point(pointCount() - 2).value;
    } else {
        
        return 0;
    }

    
}

QDateTime ExperimentData::beginTime() const
{
    

    if(m_data.isEmpty()) {
        
        return QDateTime();
    } else {
        
        return m_data.first().dateTime;
    }

    
}

QDateTime ExperimentData::endTime() const
{
    

    if(m_data.isEmpty()) {
        
        return QDateTime();
    } else {
        
        return m_data.last().dateTime;
    }

    
}

Measurements::TimeInterval ExperimentData::timeElapsed() const
{
    

    auto secondsElapsed = beginTime().secsTo(endTime());

    

    return Measurements::TimeInterval::fromHMS(0, 0, secondsElapsed);
}

void ExperimentData::clearData()
{
    

    m_data.clear();
    m_data.squeeze();

    
}
