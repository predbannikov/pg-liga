QT -= gui

CONFIG += c++11 console staticlib
TEMPLATE = lib
unix:{
    DESTDIR=$$DESTDIR_RELEASE
}
SOURCES += \
    datastore.cpp

HEADERS += \
    datastore.h
