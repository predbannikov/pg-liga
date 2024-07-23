#include "controller.h"

RETCODE Controller::readStatus(QJsonObject &jOperation)
{
    return read(jOperation, ControllerStatus);
}

RETCODE Controller::setTarget(QJsonObject &jOperation, float newtonsMin, float newtons)
{
    // 0.0004 метра квадратных
    RETCODE ret = ERROR;
    switch(stateSet) {
    case STATE_SET_TARGET_MIN_VALUE:
        ret = write(jOperation, newtonsMin);
        if (ret == COMPLATE)
            stateSet = STATE_SET_TARGET_MIN;
        else if (ret == ERROR) {
            stateSet = STATE_SET_TARGET_MIN_VALUE;
            return ERROR;
        }
        break;
    case STATE_SET_TARGET_MIN:
        ret = write(jOperation, ControllerSetMin);
        if (ret == COMPLATE)
            stateSet = STATE_SET_TARGET_VALUE;
        else if (ret == ERROR) {
            stateSet = STATE_SET_TARGET_MIN_VALUE;
            return ERROR;
        }
        break;
/* Если убрать задачу минимальной цели, то нужно stateSet по умолчанию выставить в STATE_SET_TARGET_VALUE и закоментировать код выше
   и в STATE_ACTIVATE_PID выставить STATE_SET_TARGET_VALUE
*/
    case STATE_SET_TARGET_VALUE:
        ret = write(jOperation, newtons);
        if (ret == COMPLATE)
            stateSet = STATE_SET_TARGET;
        else if (ret == ERROR) {
            stateSet = STATE_SET_TARGET_VALUE;
            return ERROR;
        }
        break;
    case STATE_SET_TARGET:
        ret = write(jOperation, ControllerSet);
        if (ret == COMPLATE)
            stateSet = STATE_ACTIVATE_PID_VALUE;
        else if (ret == ERROR) {
            stateSet = STATE_SET_TARGET_VALUE;
            return ERROR;
        }
        break;
    case STATE_ACTIVATE_PID_VALUE:
        ret = write(jOperation, 1);                      // (0x0 - stop, 0x1 - locked, 0x11 - fast
        if (ret == COMPLATE) {
            stateSet = STATE_ACTIVATE_PID;
        }
        else if (ret == ERROR) {
            stateSet = STATE_SET_TARGET_VALUE;
            return ERROR;
        }
        break;
    case STATE_ACTIVATE_PID:
        ret = write(jOperation, ControllerLock);
        if (ret == COMPLATE) {
            stateSet = STATE_SET_TARGET_MIN_VALUE;
            return COMPLATE;
        }
        else if (ret == ERROR) {
            stateSet = STATE_SET_TARGET_MIN_VALUE;
            return ERROR;
        }
        break;
    }
    return NOERROR;
}

RETCODE Controller::setKPID(QJsonObject &jOperation, float value, CMD cmd)
{
    RETCODE ret = ERROR;
    switch (state_pid) {
    case STATE_SET_PID_VALUE:
        ret = write(jOperation, value);                    // TODO
        if (ret == COMPLATE)
            state_pid = STATE_SET_PID;
        else if (ret == ERROR) {
            state_pid = STATE_SET_PID_VALUE;
            return ERROR;
        }
        break;
    case STATE_SET_PID:
        ret = write(jOperation, cmd);
        if (ret == COMPLATE) {
            state_pid = STATE_SET_PID_VALUE;
            return COMPLATE;
        }
        else if (ret == ERROR) {
            state_pid = STATE_SET_PID_VALUE;
            return ERROR;
        }
        break;
    }
    return NOERROR;
}

//RETCODE Controller::setKd(QJsonObject &jOperation, float d, CMD cmd)
//{
//    RETCODE ret = ERROR;
//    switch (state_pid_d) {
//    case STATE_SET_PID_D_VALUE:
//        ret = write(jOperation, d);                    // TODO
//        if (ret == COMPLATE)
//            state_pid_d = STATE_SET_PID_D;
//        else if (ret == ERROR) {
//            state_pid_d = STATE_SET_PID_D_VALUE;
//            return ERROR;
//        }
//        break;
//    case STATE_SET_PID_D:
//        ret = write(jOperation, cmd);
//        if (ret == COMPLATE) {
//            state_pid_d = STATE_SET_PID_D_VALUE;
//            return COMPLATE;
//        }
//        else if (ret == ERROR) {
//            state_pid_d = STATE_SET_PID_D_VALUE;
//            return ERROR;
//        }
//        break;
//    }
//    return NOERROR;
//}

RETCODE Controller::setHz(QJsonObject &jOperation, float hz)
{
    switch (stateSetHz) {
    case STATE_SET_HZ_WRITE:
        qDebug() << "STATE_SET_HZ_WRITE";
        if (write(jOperation, hz) == COMPLATE) {
            stateSetHz = STATE_SET_HZ_CMD;
            QThread::msleep(100);       // Если убрать задержку зависает , не двигается рама но отвечает успешно на комманды
        }
        break;
    case STATE_SET_HZ_CMD:
        qDebug() << "STATE_SET_HZ_CMD";
        if (write(jOperation, ControllerSetHz) == COMPLATE) {
            stateSetHz = STATE_SET_HZ_WRITE;
            QThread::msleep(100);       // Если убрать задержку зависает , не двигается рама но отвечает успешно на комманды
            return COMPLATE;
        }
        break;
    }
    return NOERROR;
}

#define DEBUG_STOP_PID 0

RETCODE Controller::stopPID(QJsonObject &jOperation)
{
    if (DEBUG_STOP_PID) {
        switch (stateUnlock) {
        case STATE_UNLOCK_START:
            ret = readStatus(jOperation);
            if (ret == COMPLATE) {
                qDebug() << "status =" << status;
                if (status != 0) {
//                    stateUnlock = STATE_UNLOCK_START;
//                    return COMPLATE;
                    static quint64 counter_status_one = 0;
                    counter_status_one++;
                    if (counter_status_one == 3) {
                        stateUnlock = STATE_UNLOCK_SET_VALUE;
                        counter_status_one = 0;
                    }
                    QThread::msleep(500);
                } else {
                    qDebug() << "### status =" << status;
                    stateUnlock = STATE_UNLOCK_START;
                    return COMPLATE;
                }
            }
            else if (ret == ERROR) {
                stateUnlock = STATE_UNLOCK_START;
                return ERROR;
            }
            break;
        case STATE_UNLOCK_SET_VALUE:
            ret = write(jOperation, 0x0);                      // (0x0 - stop, 0x1 - locked, 0x11 - fast
            if (ret == COMPLATE) {
                stateUnlock = STATE_UNLOCK_SET_CMD;
            }
            else if (ret == ERROR) {
                stateUnlock = STATE_UNLOCK_START;
                return ERROR;
            }
            break;
        case STATE_UNLOCK_SET_CMD:
            ret = write(jOperation, ControllerLock);
            if (ret == COMPLATE) {
                stateUnlock = STATE_UNLOCK_START;
                return COMPLATE;
            }
            else if (ret == ERROR) {
                stateUnlock = STATE_UNLOCK_START;
                return ERROR;
            }
            break;
        case STATE_HANDLE_ERROR:
            ret = readStatus(jOperation);
            if (ret == COMPLATE) {
                qDebug() << "status =" << status;
                stateUnlock = STATE_UNLOCK_SET_VALUE;
            } else if (ret == ERROR){
                stateUnlock = STATE_UNLOCK_SET_VALUE;
                return ERROR;
            }
            break;
        }


//=====================================================================
    } else {
        switch (stateUnlock) {
        case STATE_UNLOCK_SET_VALUE:
            ret = write(jOperation, 0x0);                      // (0x0 - stop, 0x1 - locked, 0x11 - fast
            if (ret == COMPLATE) {
                stateUnlock = STATE_UNLOCK_SET_CMD;
            }
            else if (ret == ERROR) {
                stateUnlock = STATE_HANDLE_ERROR;
                qDebug() << "request" << jOperation["request"];
                qDebug() << "response" << jOperation["response"];
                qDebug() << "###" << Q_FUNC_INFO;
            }
            break;
        case STATE_UNLOCK_SET_CMD:
            ret = write(jOperation, ControllerLock);
            if (ret == COMPLATE) {
                stateUnlock = STATE_UNLOCK_SET_VALUE;
                return COMPLATE;
            }
            else if (ret == ERROR) {
                stateUnlock = STATE_HANDLE_ERROR;
            }
            break;
        case STATE_HANDLE_ERROR:
            ret = readStatus(jOperation);
            if (ret == COMPLATE) {
                qDebug() << "status (if 1 what that is?(atomic?)) " << status;
                stateUnlock = STATE_UNLOCK_SET_VALUE;
                if (status == 0)
                    return COMPLATE;
            } else if (ret == ERROR){
                stateUnlock = STATE_UNLOCK_SET_VALUE;
                return ERROR;
            }
            break;
        }
    }
    return NOERROR;
}

void Controller::setValue(quint16 *data, AbstractUnit::CMD cmd) {
    if (cmd == ControllerStatus)
        status = qFromLittleEndian<qint32>(data);

}
