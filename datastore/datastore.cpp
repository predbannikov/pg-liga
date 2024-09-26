#include "datastore.h"
#include "math.h"

DataStore::DataStore() {

}

DataStore::~DataStore() {
    qDebug() << "test delete";
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
    for (const auto &jkeyTime: jData.keys()) {
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
    for (qint64 idx: qAsConst(list_sort))
        all_lists.append(new_map[idx]);
    return all_lists;
}

void DataStore::append(qint64 start_time, qint64 cur_time, float value, float eps) {
    if (!data.contains(start_time)) {
        data.insert(start_time, {{cur_time, value}});
    } else {
        auto lastValue = data[start_time].last().second;
        double diffValue = fabs (lastValue - value);
        if (diffValue > eps) {
            data[start_time].append({cur_time, value});
        }
    }
}

qint64 DataStore::size() {
    qint64 sz = 0;
    for (auto &lst: data) {
        sz += lst.size();
    }
    return sz;
}

QList<QPair<qint64, float>> &DataStore::getDataOfStartTime() {
    QList<qint64> keys = data.keys();
    std::sort(keys.begin(), keys.end());
    return data[keys.last()];
}

QPair<bool, float> DataStore::valueFromBackOfStepTime(qint64 step_time, qint64 timeMs) {
    auto &arr = data[step_time];
    const QPair<bool, float> pair = {false, 0};
    if (arr.isEmpty()) {
        qDebug() << Q_FUNC_INFO << "Не существует ключа шага (индекса в данном массиве)";
        return pair;
    }
    if (arr.last().first < timeMs) {
        qDebug() << Q_FUNC_INFO << "Данных с таким временем ещё не существует";
        return pair;
    }
    auto valueToFind_approx = arr.last().first - timeMs;   // Нужно найти значение не больше этого
    for (int i = arr.size() - 1; i > 0; i--) {
        if (arr[i].first <= valueToFind_approx) {
            return {true, arr[i].second};
        }
    }
    return pair;
}

QPair<bool, float> DataStore::valueFromBackOfStepTime(qint64 step_time, double timeMs)
{
    return valueFromBackOfStepTime(step_time, static_cast<qint64>(timeMs));
}

void DataStore::getLastStartAndCurTime(qint64 &start_time, qint64 &cur_time) {
    QList<qint64> keys = data.keys();
    std::sort(keys.begin(), keys.end());
    if (keys.isEmpty())
        return;
    start_time = keys.last();
    QList<QPair<qint64, float>> &lastList = data[start_time];
    if (!lastList.empty()) {
        cur_time = data[start_time].last().first;
    } else {
        cur_time = start_time;
    }
}
