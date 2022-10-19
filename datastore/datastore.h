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
    DataStore();
    ~DataStore();
    /**
     * @brief serializeData         Мап хранит значения сенсора на каждом шаге испытания. см. переменную data.
     * @param jobj                  QJsonObject объект, в котором должно находиться:
     *                              start_time - qint64 ключ для доступа к листу -> QMap<qint64, QList<QPair<qint64, float>>>
     *                                              лист хранит значения полученные на одном этапе (шаге) эксперимента
     *                              cur_time -      значение пары листа QPair<qint64, float>
     */
    void serializeData(QJsonObject &jobj);

    QList<QPair<qint64, float>> deSerializeData(QJsonObject jData);

    float valueFromBack(qint64 step_time, qint64 time);

    void append(qint64 start_time, qint64 cur_time, float value, float eps = 0.01);

    void fixAppend(qint64 start_time, qint64 cur_time, float value);

    qint64 size();

    void getLastStartAndCurTime(qint64 &start_time, qint64 &cur_time);

    QMap <qint64, QList<QPair<qint64, float>>> data;
};

#endif // DATASTORE_H
