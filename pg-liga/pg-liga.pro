QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += qwt
win32 {
    msvc: LIBS += -LC:\Qwt-6.1.3\lib-msvc-compiller17.9
    else: win32-g++: LIBS += -LC:\Qwt-6.1.3\lib
    CONFIG(release, debug|release) {
        LIBS += -lqwt
    } else {
        LIBS += -lqwtd
    }
    INCLUDEPATH += C:\Qwt-6.1.3\include
}
#QMAKE_CXXFLAGS += -Wextra

TEMPLATE = app
unix:{
    DESTDIR=$$DESTDIR_RELEASE
}
#TARGET = liga

VERSION_MAJOR=1
VERSION_MINOR=0
VERSION_BUILD=$$system(git rev-list HEAD --count)
DEFINES += VERSION_MAJOR=\\\"$$VERSION_MAJOR\\\" \
           VERSION_MINOR=\\\"$$VERSION_MINOR\\\" \
           VERSION_BUILD=\\\"$$VERSION_BUILD\\\" \

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    LogHelper/log_helper.cpp \
    Settings/settingsmanager.cpp \
    Settings/settingsdialog.cpp     \
    clientmanager.cpp \
    experimentview.cpp \
	decoratedplot.cpp \
	experimentdata.cpp \
	qwtchartzoom.cpp \
	qwheelzoomsvc.cpp \
	qaxiszoomsvc.cpp \
	plotadapter.cpp \
	plotxyadapter.cpp \
    main.cpp \
    mainwindow.cpp \
    serverwindow.cpp


HEADERS += \
    LogHelper/log_helper.h \
    LogHelper/log_helper_cfg.h \
    Settings/instrumentparameters.h \
    Settings/settingsmanager.h \
    Settings/settingsdialog.h   \
    abstract/abstractjsonclient.h \
    Physics/measurements.h \
    clientmanager.h \
    experimentview.h \
	experimentdata.h \
	decoratedplot.h \
	qwtchartzoom.h \
	qwheelzoomsvc.h \
	qaxiszoomsvc.h \
	plotadapter.h \
	plotxyadapter.h \
    mainwindow.h \
    serverwindow.h

FORMS += \
    experimentview.ui \
    mainwindow.ui   \
    Settings/settingsdialog.ui \
    serverwindow.ui

TRANSLATIONS += \
    pg-liga_ru_RU.ts

RESOURCES += \
    resources/resources.qrc

RC_ICONS += \
    resources/icons/appicon.ico

INCLUDEPATH += $$PWD/../

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../network/release/ -lnetwork
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../network/debug/ -lnetwork
else:unix: LIBS += -L$$DESTDIR -lnetwork

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../datastore/release/ -ldatastore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../datastore/debug/ -ldatastore
else:unix: LIBS += -L$$DESTDIR -ldatastore

