#include "datastore.h"

DataStore::DataStore() {

}

DataStore::~DataStore() {

}

void DataStore::serializeData(QJsonObject &jobj) {
    qint64 start_time = jobj["start_time"].toString().toInt();
    qint64 cur_time = jobj["cur_time"].toString().toInt();
    jobj = QJsonObject();

    QMap<qint64, QList<QPair<qint64, float>>>::iterator itStartTime = data.begin();
    for (; itStartTime != data.end(); itStartTime++) {
        QByteArray buff;
        QDataStream ds(&buff, QIODevice::ReadWrite);
        ds.setVersion(QDataStream::Qt_5_11);
        ds.setByteOrder(QDataStream::BigEndian);
        if (start_time == itStartTime.key()) {
            auto &src_list = itStartTime.value();
            int cur_pos_list = 0;
            while (src_list.size() > cur_pos_list && src_list[cur_pos_list++].first < cur_time);
            auto prep_list = src_list.mid(cur_pos_list);
            ds << prep_list;
            jobj[QString::number(itStartTime.key())] = QString(buff.toBase64());
        } else if (start_time < itStartTime.key()) {
            ds << itStartTime.value();
            jobj[QString::number(itStartTime.key())] = QString(buff.toBase64());                   // list pairs
        }
    }
}

QList<QPair<qint64, float> > DataStore::deSerializeData(QJsonObject jData) {
    QMap<qint64, QList<QPair<qint64, float>>> new_map;
    for (auto jkeyTime: jData.keys()) {
        QByteArray buff = QByteArray::fromBase64(jData[jkeyTime].toString().toUtf8());
        QDataStream ds(&buff, QIODevice::ReadOnly);
        ds.setVersion(QDataStream::Qt_5_11);
        ds.setByteOrder(QDataStream::BigEndian);
        QList<QPair<qint64, float>> list;
        ds >> list;
        new_map.insert(jkeyTime.toInt(), list);
        if (data.contains(jkeyTime.toInt())) {
            data[jkeyTime.toInt()].append(list);
        } else {
            data.insert(jkeyTime.toInt(), list);
        }
    }
    QList <QPair <qint64, float>> all_lists;
    QList <qint64> list_sort = new_map.keys();
    std::sort(list_sort.begin(), list_sort.end());
    for (qint64 idx: list_sort)
        all_lists.append(new_map[idx]);
    return all_lists;
}

float DataStore::valueFromBack(qint64 step_time, qint64 time) {
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

void DataStore::append(qint64 start_time, qint64 cur_time, float value, float eps) {
    if (!data.contains(start_time)) {
        data.insert(start_time, {{cur_time, value}});
    } else {
        double diffValue = fabs (data[start_time].last().second - (value));
        if (diffValue > eps) {
            data[start_time].append({cur_time, value});
//            if (last.first != data[start_time].last().first)
//                data[start_time].append(last);
        } else {
//            qDebug() << Q_FUNC_INFO << "Значение изменения ещё меньше";
        }
//        last = {cur_time, value};
    }
}

void DataStore::fixAppend(qint64 start_time, qint64 cur_time, float value)
{
    if (!data.contains(start_time)) {
        data.insert(start_time, {{cur_time, value}});
    } else {
        data[start_time].append({cur_time, value});
    }
}

qint64 DataStore::size() {
    qint64 sz = 0;
    for (auto &lst: data) {
        sz += lst.size();
    }
    return sz;
}

void DataStore::getLastStartAndCurTime(qint64 &start_time, qint64 &cur_time) {
    QList<qint64> keys = data.keys();
    std::sort(keys.begin(), keys.end());
    start_time = keys.last();
    QList<QPair<qint64, float>> &lastList = data[start_time];
    if (!lastList.empty()) {
        cur_time = data[start_time].last().first;
    } else {
        cur_time = start_time;
    }
}
