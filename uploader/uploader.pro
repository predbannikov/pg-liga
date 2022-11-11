QT -= gui
QT += network

CONFIG += c++17 console
CONFIG -= app_bundle
unix: DESTDIR=$$DESTDIR_RELEASE


VERSION_MAJOR=1
VERSION_MINOR=0
VERSION_BUILD=$$system(git rev-list HEAD --count)
DEFINES += VERSION_MAJOR=\\\"$$VERSION_MAJOR\\\" \
           VERSION_MINOR=\\\"$$VERSION_MINOR\\\" \
           VERSION_BUILD=\\\"$$VERSION_BUILD\\\" \


SOURCES += \
        clientrepositarion.cpp \
        main.cpp \
        uploader.cpp


INCLUDEPATH += $$PWD/../
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../network/release/ -lnetwork
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../network/debug/ -lnetwork
else:unix: LIBS += -L$$DESTDIR -lnetwork



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    clientrepositarion.h \
    uploader.h
