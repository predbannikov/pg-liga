QT -= gui

CONFIG += c++11 console staticlib
TEMPLATE = lib

SOURCES += \
    datastore.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    datastore.h
