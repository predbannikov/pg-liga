#ifndef EXPERIMENTDATA_H
#define EXPERIMENTDATA_H

#include <QObject>
#include <QVector>
#include <QDateTime>

#include "measurements.h"

class ExperimentData : public QObject
{
    Q_OBJECT

public:
    struct Point {
        double value;
        QDateTime dateTime;

        bool isValid() const { return dateTime.isValid(); }
    };

    ExperimentData(QObject *parent = nullptr);

    void append(double value, const QDateTime &dateTime = QDateTime::currentDateTime());
    void append(QList<QPair<qint64, float>> &list);

    Point point(int idx) const { return m_data.at(idx); }
    Point pointLast() const { return m_data.last(); }
    Point pointBackInTime(Measurements::TimeInterval time) const;
    Point pointFromLast(int idx) const { return *(m_data.crbegin() + idx); }

    int pointCount() const { return m_data.size(); }
    void setPoint(const Point &point, int idx);

    operator QVector<double>() const;

    double minValue() const;
    double maxValue() const;
    double lastDelta() const;

    QDateTime beginTime() const;
    QDateTime endTime() const;
    Measurements::TimeInterval timeElapsed() const;

    void clear() { m_data.clear(); }
    void clearData();

    void setOffset(double offset) { m_offset = offset; }

signals:
    void pointInserted();

private:
    QVector<Point> m_data;
    double m_offset = 0;
};

#endif // EXPERIMENTDATA_H
