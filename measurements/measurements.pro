QT -= gui
CONFIG += c++11 console staticlib
TEMPLATE = lib
unix:{
    DESTDIR=$$DESTDIR_RELEASE
}

SOURCES += \

HEADERS += \
        measurements.h

