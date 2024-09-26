#ifndef DATASTORE_H
#define DATASTORE_H

#include <QMap>
#include <QDebug>
#include <QJsonObject>
#include <QDataStream>
#include <algorithm>

class DataStore
{
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

    /**
     * @brief valueFromBackOfStepTime
     * @param step_time                 По какому индексу (индекс записывается по времени) выбирать массив
     * @param time                      Вернуть значение из массива, time времени назад
     * @return                          QPair<bool, float>  bool указывает на валидность выполнения
     *                                                      float содержит значение
     *                                                      В случае не успеха выполнения, float содержит первое значение с конца массива
     */
    QPair<bool, float> valueFromBackOfStepTime(qint64 step_time, qint64 time);
    QPair<bool, float> valueFromBackOfStepTime(qint64 step_time, double time);

    /**
     * @brief append            Добавить точку в хранилище
     * @param start_time        время начала ступени
     * @param cur_time          текущее время
     * @param value
     * @param eps
     */
    void append(qint64 start_time, qint64 cur_time, float value, float eps = 0.00001);

    qint64 size();

    void getLastStartAndCurTime(qint64 &start_time, qint64 &cur_time);
    QList<QPair<qint64, float> > &getDataOfStartTime();
    QMap <qint64, QList<QPair<qint64, float>>> data;
};

#endif // DATASTORE_H
