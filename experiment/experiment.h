#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "operations.h"

#include "baseaction.h"

/**
 * @brief The Experiment class      После старта состояния процесса, запускается conveyor
 *      который имеет несколько состояний, первое состояние проверяет jExperiment
 *      сразу же переходит во второе состояние в случае успеха иначе запускает процесс
 *      перехода эксперимента в состояние IDLE. Второе состояние перебирает все доступные
 *      Operations (Actions) и ищет совпадение с прочитанным ранее значением curAction.
 *      После успешного совпадения, создаётся Action и запускается третье состояние иначе
 *      запускается процесс перехода в состояние конца эксперимента (которое завершается паузой)
 *      Третье состояние запускает процесс обновления экшиона до тех пор пока он не вернёт
 *      true, что значит экшион выполнен.
 *
 *
 *
 *
 *      объект jStatus из jExperiment["status"], в котором находятся:
 *      "curAction":"%1" - число указывающее на номер операции которую необходимо выбрать из
 *          списка операций и запустить
 *      "state":"%1" - наименование текущего состояния указываются только idle, process, pause
 *
 */


class Experiment : public Operations
{
    Q_OBJECT

    enum TRANSITION {TRANSITION_1, TRANSITION_2, TRANSITION_3, TRANSITION_4} transition = TRANSITION_1;
    enum TRANSITION_TO_PAUSE {TRANSITION_TO_PAUSE_1, TRANSITION_TO_PAUSE_2} transitionToPause = TRANSITION_TO_PAUSE_1;
    enum TRANSITION_TO_STOP {TRANSITION_TO_STOP_1, TRANSITION_TO_STOP_2} transitionToStop = TRANSITION_TO_STOP_1;

    // Перечисление создано для управления устройством, физичиского остановления рамы , возможно стоит перенести реализацию в Operations, а оператионс познее переименовать
    enum TRANSITION_TO_STOP_DEVICE {
        TRANSITION_TO_STOP_LOADFRAME_1, TRANSITION_TO_STOP_LOADFRAME_2,
        TRANSITION_TO_STOP_VOLUMETER1_1, TRANSITION_TO_STOP_VOLUMETER1_2,
        TRANSITION_TO_STOP_VOLUMETER2_1, TRANSITION_TO_STOP_VOLUMETER2_2,
    } transitionToStopDevice = TRANSITION_TO_STOP_LOADFRAME_1;

public:
    Experiment(QString serial_port_name, quint8 addr);
    ~Experiment();
    void stateSwitch() override;

public slots:
    void doWork();


private:
    bool conveyor();
    bool pausing();
    bool stopping();
    bool stopDevice();
    bool createAction(QJsonObject jAction);
    void deleteAction();
    void updateJExperiment();


    // Работа с jExperiment
    void jIncCurAction();                               // Устанавливает указатель на следующую операцию. Если такой операции не будет, то эксперимент приостановится
    void jUpdateExperimentAction(QJsonObject jObj);     // Записываем в jExperiment jAction
    void jSaveState(QString state);                     // Записывает состояние эксперимента в jExperiment
    QString curAction();                                // Возвращает текущую операцию

    int loadFramePosition = 0;
    int volumeter1Position = 0;
    int volumeter2Position = 0;
    BaseAction *action = nullptr;
    QString portName;
};

#endif // EXPERIMENT_H
