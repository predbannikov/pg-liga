TEMPLATE = subdirs


SUBDIRS += \
 datastore \
 network \
 experiment \
 experiment-manager \
 modbus-server \
 repositarion-server \
 measurements

enable-gui-client {
  message(gui client enabled)
  SUBDIRS += pg-liga
}
