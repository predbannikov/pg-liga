#include "controller.h"

RETCODE Controller::readStatus(StatusOperation &operation)
{
    return read(operation, ControllerStatus);
}

RETCODE Controller::setTarget(StatusOperation &operation, float newtons)
{
    // 0.0004 метра квадратных
    RETCODE ret = ERROR;
    switch(stateSet) {
    case STATE_SET_TARGET_VALUE:
        ret = write(operation, newtons);
        if (ret == COMPLATE)
            stateSet = STATE_SET_TARGET;
        else if (ret == ERROR) {
            stateSet = STATE_SET_TARGET_VALUE;
            return ERROR;
        }
        break;
    case STATE_SET_TARGET:
        ret = write(operation, ControllerSet);
        if (ret == COMPLATE)
            stateSet = STATE_SET_PID_P_VALUE;
        else if (ret == ERROR) {
            stateSet = STATE_SET_TARGET_VALUE;
            return ERROR;
        }
        break;
    case STATE_SET_PID_P_VALUE:
        ret = write(operation, 7.5);                    // TODO
        if (ret == COMPLATE)
            stateSet = STATE_SET_PID_P;
        else if (ret == ERROR) {
            stateSet = STATE_SET_TARGET_VALUE;
            return ERROR;
        }
        break;
    case STATE_SET_PID_P:
        ret = write(operation, ControllerSetKp);
        if (ret == COMPLATE)
            stateSet = STATE_SET_PID_D_VALUE;
        else if (ret == ERROR) {
            stateSet = STATE_SET_TARGET_VALUE;
            return ERROR;
        }
        break;
    case STATE_SET_PID_D_VALUE:
        ret = write(operation, 75.);                    // TODO
        if (ret == COMPLATE)
            stateSet = STATE_SET_PID_D;
        else if (ret == ERROR) {
            stateSet = STATE_SET_TARGET_VALUE;
            return ERROR;
        }
        break;
    case STATE_SET_PID_D:
        ret = write(operation, ControllerSetKd);
        if (ret == COMPLATE) {
            stateSet = STATE_ACTIVATE_PID_VALUE;
        }
        else if (ret == ERROR) {
            stateSet = STATE_SET_TARGET_VALUE;
            return ERROR;
        }
        break;
    case STATE_ACTIVATE_PID_VALUE:
        ret = write(operation, 1);                      // (0x0 - stop, 0x1 - locked, 0x11 - fast
        if (ret == COMPLATE) {
            stateSet = STATE_ACTIVATE_PID;
        }
        else if (ret == ERROR) {
            stateSet = STATE_SET_TARGET_VALUE;
            return ERROR;
        }
        break;
    case STATE_ACTIVATE_PID:
        ret = write(operation, ControllerLock);
        if (ret == COMPLATE) {
            stateSet = STATE_SET_TARGET_VALUE;
            return COMPLATE;
        }
        else if (ret == ERROR) {
            stateSet = STATE_SET_TARGET_VALUE;
            return ERROR;
        }
        break;
    }
    return NOERROR;
}

#define DEBUG_STOP_PID 0

RETCODE Controller::stopPID(StatusOperation &operation)
{
    if (DEBUG_STOP_PID) {
        switch (stateUnlock) {
        case STATE_UNLOCK_START:
            ret = readStatus(operation);
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
            ret = write(operation, 0x0);                      // (0x0 - stop, 0x1 - locked, 0x11 - fast
            if (ret == COMPLATE) {
                stateUnlock = STATE_UNLOCK_SET_CMD;
            }
            else if (ret == ERROR) {
                stateUnlock = STATE_UNLOCK_START;
                return ERROR;
            }
            break;
        case STATE_UNLOCK_SET_CMD:
            ret = write(operation, ControllerLock);
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
            ret = readStatus(operation);
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
            ret = write(operation, 0x0);                      // (0x0 - stop, 0x1 - locked, 0x11 - fast
            if (ret == COMPLATE) {
                stateUnlock = STATE_UNLOCK_SET_CMD;
            }
            else if (ret == ERROR) {
                stateUnlock = STATE_HANDLE_ERROR;
                qDebug() << "request" << operation.request;
                qDebug() << "response" << operation.response;
                qDebug() << "###" << Q_FUNC_INFO;
            }
            break;
        case STATE_UNLOCK_SET_CMD:
            ret = write(operation, ControllerLock);
            if (ret == COMPLATE) {
                stateUnlock = STATE_UNLOCK_SET_VALUE;
                return COMPLATE;
            }
            else if (ret == ERROR) {
                stateUnlock = STATE_HANDLE_ERROR;
            }
            break;
        case STATE_HANDLE_ERROR:
            ret = readStatus(operation);
            if (ret == COMPLATE) {
                qDebug() << "status (if 1 what that is?(atomic?)) " << status;
                stateUnlock = STATE_UNLOCK_SET_VALUE;
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
