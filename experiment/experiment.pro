QT -= gui
QT += network serialport

CONFIG += c++17 console
CONFIG -= app_bundle

TEMPLATE = app
unix: DESTDIR=$$DESTDIR_RELEASE

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        abstractunit.cpp \
        actions/adjustloadframe.cpp \
        actions/movebytimeloadframe.cpp \
        actions/steppedpressure.cpp \
        baseaction.cpp \
        clientmanager.cpp \
        controller.cpp \
        experiment.cpp \
        interface.cpp \
        loadframe.cpp \
        operations.cpp \
        sensors.cpp \
        serialport.cpp \
        server.cpp  \
        main.cpp \
        stepper.cpp \
        storedata.cpp \
        volumeter1.cpp \
        volumeter2.cpp

HEADERS += \
        abstractunit.h \
        actions/adjustloadframe.h \
        actions/movebytimeloadframe.h \
        actions/steppedpressure.h \
        baseaction.h \
        clientmanager.h \
        controller.h \
        experiment.h    \
        global.h \
        interface.h \
        loadframe.h \
        operations.h \
        sensors.h \
        serialport.h \
        server.h    \
        stepper.h \
        storedata.h \
        volumeter1.h \
        volumeter2.h

#INCLUDEPATH += $$PWD/../
INCLUDEPATH += $$PWD/../measurements/
INCLUDEPATH += $$PWD/../datastore/
INCLUDEPATH += $$PWD/../network/
INCLUDEPATH += $$PWD/actions/

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../network/release/ -lnetwork
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../network/debug/ -lnetwork
#else:unix: LIBS += -L$$DESTDIR -lnetwork
else:unix: LIBS += -L$$OUT_PWD/../network -lnetwork

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../datastore/release/ -ldatastore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../datastore/debug/ -ldatastore
#else:unix: LIBS += -L$$DESTDIR -ldatastore
else:unix: LIBS += -L$$OUT_PWD/../datastore -ldatastore

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../measurements/release/ -lmeasurements
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../measurements/debug/ -lmeasurements
#else:unix: LIBS += -L$$DESTDIR -ldatastore
else:unix: LIBS += -L$$OUT_PWD/../measurements -lmeasurements


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
