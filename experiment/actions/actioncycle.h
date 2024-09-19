#ifndef ACTIONCYCLE_H
#define ACTIONCYCLE_H

#include <QObject>

#include "baseaction.h"

class ActionCycle : public BaseAction
{
    Q_OBJECT

public:
    explicit ActionCycle(QObject *parent = nullptr);
    ~ActionCycle();

    virtual bool updateSteping() = 0;
    virtual bool stepChanged() = 0;

    void init() override;
    bool update() override;
    void finishing() override;
    void pausing() override;


    // Работа с jActionCycle
    /**
     * @brief jIncCurStep           Устанавливает указатель на следующий шаг. Если такого шага не будет, то
     *                              возвращаем true, что вызовет finishing
     */
    void jIncCurStep();

    /**
     * @brief jUpdateJAction        Записываем в jAction параметры если были изменения в данных
     * @param jObj
     */
    void jUpdateJAction(QJsonObject jObj);

    /**
     * @brief jSaveState            Записывает состояние эксперимента в jAction
     * @param state
     */
    void jSaveState(QString state);

    /**
     * @brief curStep               // Возвращает текущий Step
     * @return
     */
    QString curStep();

    QJsonObject jStep;
private:

    QTimer elapseTime;
    enum TRANS {TRANSITION_1,
                TRANSITION_2,
                TRANSITION_3,
                TRANSITION_4,
               } trans = TRANSITION_1;

};

#endif // ACTIONCYCLE_H
