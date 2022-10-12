#ifndef DATASTORE_H
#define DATASTORE_H

#include <QMap>
#include <QDebug>
#include <QJsonObject>
#include <QDataStream>
#include <algorithm>

class DataStore
{
    QPair <qint64, float> last;
public:
    DataStore() {

    };
    ~DataStore() {

    };
    /**
     * @brief serializeData         Мап хранит значения сенсора на каждом шаге испытания. см. переменную data.
     * @param jobj                  QJsonObject объект, в котором должно находиться:
     *                              start_time - qint64 ключ для доступа к листу -> QMap<qint64, QList<QPair<qint64, float>>>
     *                                              лист хранит значения полученные на одном этапе (шаге) эксперимента
     *                              cur_time -      значение пары листа QPair<qint64, float>
     */
    void serializeData(QJsonObject &jobj) {
        qint64 start_time = jobj["start_time"].toString().toInt();
        qint64 cur_time = jobj["cur_time"].toString().toInt();
        jobj = QJsonObject();

        auto itStartTime = data.begin();
        for (; itStartTime != data.end(); itStartTime++) {
            QByteArray buff;
            QDataStream ds(&buff, QIODevice::ReadWrite);
            ds.setVersion(QDataStream::Qt_5_11);
            ds.setByteOrder(QDataStream::BigEndian);
            if (start_time == itStartTime.key()) {
                auto &src_list = itStartTime.value();
                int start_pos_list = 0;
                while (src_list[start_pos_list++].first < cur_time);
                auto prep_list = src_list.mid(start_pos_list);
                ds << prep_list;
                jobj[QString::number(itStartTime.key())] = QString(buff.toBase64());
            } else if (start_time < itStartTime.key()) {
                ds << itStartTime.value();
                jobj[QString::number(itStartTime.key())] = QString(buff.toBase64());                   // list pairs
            }
        }
    }

    QList<QPair<qint64, float>> deSerializeData(QJsonObject jData) {
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

    void getLastStartAndCurTime(qint64 &start_time, qint64 &cur_time) {
        auto keys = data.keys();
        std::sort(keys.begin(), keys.end());
        start_time = keys.last();
        cur_time = data[start_time].last().first;
    }

    QMap <qint64, QList<QPair<qint64, float>>> data;
};

#endif // DATASTORE_H
