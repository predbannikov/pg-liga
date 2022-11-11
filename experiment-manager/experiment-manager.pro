QT -= gui
QT += network

CONFIG += c++11 console
CONFIG -= app_bundle

TEMPLATE = app
DESTDIR=$$DESTDIR_RELEASE

#TARGET = experimentmanager
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#CONFIG += c++11

VERSION_MAJOR=1
VERSION_MINOR=0
VERSION_BUILD=$$system(git rev-list HEAD --count)
DEFINES += VERSION_MAJOR=\\\"$$VERSION_MAJOR\\\" \
           VERSION_MINOR=\\\"$$VERSION_MINOR\\\" \
           VERSION_BUILD=\\\"$$VERSION_BUILD\\\" \

SOURCES += \
    clientexperiment.cpp \
    clientmanager.cpp \
    clientrepositarion.cpp \
    server.cpp  \
    main.cpp

HEADERS += \
    clientexperiment.h \
    clientmanager.h \
    clientrepositarion.h \
    server.h


INCLUDEPATH += $$PWD/../
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../network/release/ -lnetwork
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../network/debug/ -lnetwork
else:unix: LIBS += -L$$DESTDIR -lnetwork



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
