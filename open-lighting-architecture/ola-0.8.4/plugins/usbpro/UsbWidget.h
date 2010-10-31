/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * UsbWidget.h
 * Read and Write to a USB Widget.
 * Copyright (C) 2010 Simon Newton
 */

#ifndef PLUGINS_USBPRO_USBWIDGET_H_
#define PLUGINS_USBPRO_USBWIDGET_H_

#include <stdint.h>
#include <string>
#include "ola/network/Socket.h"
#include "olad/PluginAdaptor.h"

namespace ola {
namespace plugin {
namespace usbpro {

class UsbWidget;

/*
 * Implement this to respond to widget messages.
 */
class WidgetListener {
  public:
    virtual ~WidgetListener() {}
    virtual void HandleMessage(UsbWidget *widget,
                               uint8_t label,
                               unsigned int length,
                               const uint8_t *data) = 0;
};


/*
 * Abstract away the interface to the select server
 */
class SelectServerAdaptor {
  public:
    virtual ~SelectServerAdaptor() {}
    virtual bool AddSocket(ola::network::ConnectedSocket *socket,
                           bool delete_on_close = false) const = 0;
};


/*
 * The basic representation of a widget. This knows how to send and recieve usb
 * pro messages.
 */
class UsbWidget {
  public:
    UsbWidget(const SelectServerAdaptor &ss_adaptor, int fd);
    ~UsbWidget();
    void SetMessageHandler(WidgetListener *listener) { m_listener = listener; }
    void SetOnRemove(ola::SingleUseClosure<void> *on_close);

    void SocketReady();

    bool SendMessage(uint8_t label, unsigned int length,
                     const uint8_t *data) const;

    static const uint8_t DMX_LABEL = 6;
    static const uint8_t SERIAL_LABEL = 10;
    static const uint8_t MANUFACTURER_LABEL = 77;
    static const uint8_t DEVICE_LABEL = 78;

  private:
    typedef enum {
      PRE_SOM,
      RECV_LABEL,
      RECV_SIZE_LO,
      RECV_SIZE_HI,
      RECV_BODY,
      RECV_EOM,
    } receive_state;

    enum {MAX_DATA_SIZE = 600};

    typedef struct {
      uint8_t som;
      uint8_t label;
      uint8_t len;
      uint8_t len_hi;
    } message_header;

    WidgetListener *m_listener;
    ola::network::DeviceSocket *m_socket;
    receive_state m_state;
    unsigned int m_bytes_received;
    message_header m_header;
    uint8_t m_recv_buffer[MAX_DATA_SIZE];

    void ReceiveMessage();

    static const uint8_t EOM = 0xe7;
    static const uint8_t SOM = 0x7e;
};
}  // usbpro
}  // plugin
}  // ola
#endif  // PLUGINS_USBPRO_USBWIDGET_H_
