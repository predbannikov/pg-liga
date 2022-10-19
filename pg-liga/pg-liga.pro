QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += qwt

QMAKE_CXXFLAGS += -Wextra


TEMPLATE = app
DESTDIR=$$DESTDIR_RELEASE
TARGET = liga

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
else:unix: {
    LIBS += -L$$DESTDIR -lnetwork -ldatastore
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
