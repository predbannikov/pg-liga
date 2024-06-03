TEMPLATE = subdirs


SUBDIRS += \
 datastore \
 network \
 experiment \
 experiment-manager \
# modbus-server \
 repositarion-server \
 measurements \
 uploader

enable-gui-client {
  message(gui_client_enabled)
  SUBDIRS += pg-liga
}
