QT -= gui
QT += network

CONFIG += c++11 console
CONFIG -= app_bundle
TEMPLATE = app

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        abstractunit.cpp \
#        actions/abstractaction.cpp \
#        actions/actiongetid.cpp \
#        actions/actionstepperspeed.cpp \
#        actions/actionstepperstop.cpp \
#        actions/actiontest.cpp \
        controller.cpp \
        device.cpp \
        experiment.cpp \
        interface.cpp \
        operations.cpp \
        sensors.cpp \
        sequenceexecutor.cpp \
        server.cpp  \
        socketclientmanager.cpp \
        main.cpp \
        socketclientmodbus.cpp \
        stepper.cpp \
        storedata.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    abstractunit.h \
#    actions/abstractaction.h \
#    actions/actiongetid.h \
#    actions/actionstepperspeed.h \
#    actions/actionstepperstop.h \
#    actions/actiontest.h \
    controller.h \
    device.h \
    experiment.h    \
    global.h \
    interface.h \
    operations.h \
    sensors.h \
    sequenceexecutor.h \
    measurements.h  \
    server.h    \
    socketclientmanager.h \
    socketclientmodbus.h \
    stepper.h \
    storedata.h
