QT -= gui
QT += network

CONFIG += c++11 console
CONFIG -= app_bundle

TEMPLATE = app
DESTDIR=$$DESTDIR_RELEASE

#TARGET = experimentmanager
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    clientexperiment.cpp \
    clientmanager.cpp \
    server.cpp  \
    main.cpp

HEADERS += \
    clientexperiment.h \
    clientmanager.h \
    server.h


INCLUDEPATH += $$PWD/../
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../network/release/ -lnetwork
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../network/debug/ -lnetwork
else:unix: LIBS += -L$$DESTDIR -lnetwork



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
