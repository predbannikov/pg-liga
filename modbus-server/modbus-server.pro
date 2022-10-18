QT += network serialport core
QT -= gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
TEMPLATE = app
DESTDIR=$$DESTDIR_RELEASE


SOURCES += \
        main.cpp \
        serialport.cpp \
        server.cpp \
        socketclient.cpp


HEADERS += \
    serialport.h \
    server.h \
    socketclient.h


INCLUDEPATH += $$PWD/../
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../network/release/ -lnetwork
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../network/debug/ -lnetwork
else:unix: LIBS += -L$$OUT_PWD/../network/ -lnetwork


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
