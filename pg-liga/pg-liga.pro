QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += qwt
win32 {
#    msvc: LIBS += -LC:\Qwt-6.1.3\lib-msvc-compiller17.9
    msvc: LIBS += -LC:\Qwt-6.1.3\lib-msvc
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
    clientwindow.cpp \
    customgraph.cpp \
    experimentview.cpp \
    decoratedplot.cpp \
    experimentdata.cpp \
    models/advancedkinematicloadingmodel.cpp \
    models/advancedsaturationmodel.cpp \
    models/consolidationaltmodel.cpp \
    models/consolidationmodel.cpp \
    models/deformmodel.cpp \
    models/kinematicloadingmodel.cpp \
    models/kinematicpressurisedloadingmodel.cpp \
    models/maxdeformmodel.cpp \
    models/maxpressuremodel.cpp \
    models/relaxationloadingmodel.cpp \
    models/steppedloadingmodel.cpp \
    models/steppedpressurisemodel.cpp \
    operationactions.cpp \
    plotadapter.cpp \
    plotxyadapter.cpp \
    main.cpp \
    mainwindow.cpp \
    steppedmodeleditor.cpp \
    viewdelegate.cpp


HEADERS += \
    LogHelper/log_helper.h \
    LogHelper/log_helper_cfg.h \
    Settings/instrumentparameters.h \
    Settings/settingsmanager.h \
    Settings/settingsdialog.h   \
    abstract/abstractjsonclient.h \
    appstrings.h \
    clientmanager.h \
    clientwindow.h \
    customgraph.h \
    experimentview.h \
    experimentdata.h \
    decoratedplot.h \
    models/abstractsteppedmodel.h \
    models/advancedkinematicloadingmodel.h \
    models/advancedsaturationmodel.h \
    models/consolidationaltmodel.h \
    models/consolidationmodel.h \
    models/deformmodel.h \
    models/kinematicloadingmodel.h \
    models/kinematicpressurisedloadingmodel.h \
    models/maxdeformmodel.h \
    models/maxpressuremodel.h \
    models/relaxationloadingmodel.h \
    models/steppedloadingmodel.h \
    models/steppedpressurisemodel.h \
    operationactions.h \
    plotadapter.h \
    plotxyadapter.h \
    mainwindow.h \
    steppedmodeleditor.h \
    viewdelegate.h

FORMS += \
    clientwindow.ui \
    customgraph.ui \
    experimentview.ui \
    mainwindow.ui   \
    Settings/settingsdialog.ui \
    operationactions.ui \
    steppedmodeleditor.ui

TRANSLATIONS += \
    pg-liga_ru_RU.ts

RESOURCES += \
    resources/resources.qrc

RC_ICONS += \
    resources/icons/appicon.ico

INCLUDEPATH += $$PWD/../measurements/
INCLUDEPATH += $$PWD/../datastore/
INCLUDEPATH += $$PWD/../network/
INCLUDEPATH += $$PWD/models/

message($$HEADERS)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../network/release/ -lnetwork
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../network/debug/ -lnetwork
else:unix: LIBS += -L$$DESTDIR -lnetwork

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../datastore/release/ -ldatastore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../datastore/debug/ -ldatastore
else:unix: LIBS += -L$$DESTDIR -ldatastore

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../measurements/release/ -lmeasurements
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../measurements/debug/ -lmeasurements
else:unix: LIBS += -L$$OUT_PWD/../measurements -lmeasurements

DISTFILES += \
    resources/icons/duplicate.png



