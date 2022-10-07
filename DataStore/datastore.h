#ifndef DATASTORE_H
#define DATASTORE_H

#include <QMap>
#include <QDebug>

class DataStore
{
    QPair <qint64, float> last;
public:
    DataStore() {

    };
    ~DataStore() {

    };

    float valueFromBack(qint64 step_time, qint64 time) {
        auto &arr = data[step_time];
//        QPair <qint64, float> last_value = arr.last();
        for (int i = arr.size() - 1; i > 0; i--) {
//            if (last_value.first - arr[i].first >= time) {
            if (last.first - arr[i].first >= time) {
                return arr[i].second;
            }
        }
        qDebug() << Q_FUNC_INFO << "#WARNING: get first value stop";
        return arr.first().second;
    }

    void append(qint64 start_time, qint64 cur_time, float value, float eps = 0.01) {
        if (!data.contains(start_time)) {
            data.insert(start_time, {{cur_time, value}});
        } else {
            if (abs (data[start_time].last().second - (value)) > eps) {
                if (last.first != data[start_time].last().first)
                    data[start_time].append(last);
                data[start_time].append({cur_time, value});
            }
            last = {cur_time, value};
        }
    }

    qint64 size() {
        qint64 sz = 0;
        for (auto &lst: data) {
            sz += lst.size();
        }
        return sz;
    }
    QMap <qint64, QList<QPair<qint64, float>>> data;
};

#endif // DATASTORE_H
