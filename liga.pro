TEMPLATE = subdirs


SUBDIRS += \
 DataStore \
 network \
 experiment \
 experiment-manager \
 modbus-server \
 measurements

enable-gui-client {
  message(gui client enabled)
  SUBDIRS += pg-liga
}
