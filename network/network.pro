QT -= gui
QT += network

CONFIG += c++11 console staticlib
TEMPLATE = lib
DESTDIR=$$DESTDIR_RELEASE

SOURCES += \
        abstractjsonclient.cpp
HEADERS += \
        abstractjsonclient.h
