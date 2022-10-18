QT -= gui
QT += network

CONFIG += c++17 console
CONFIG -= app_bundle

TEMPLATE = app
DESTDIR=$$DESTDIR_RELEASE

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        abstractunit.cpp \
		clientmanager.cpp \
        controller.cpp \
        device.cpp \
        experiment.cpp \
        interface.cpp \
        operations.cpp \
        sensors.cpp \
        sequenceexecutor.cpp \
        server.cpp  \
        main.cpp \
        socketclientmodbus.cpp \
        stepper.cpp \
        storedata.cpp

HEADERS += \
    abstractunit.h \
	clientmanager.h \
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
    socketclientmodbus.h \
    stepper.h \
    storedata.h


INCLUDEPATH += $$PWD/../
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../network/release/ -lnetwork
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../network/debug/ -lnetwork
else:unix: LIBS += -L$$OUT_PWD/../network/ -lnetwork


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
