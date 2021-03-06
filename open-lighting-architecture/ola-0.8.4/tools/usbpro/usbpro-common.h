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
 * usbpro-common.cpp
 * Copyright (C) 2010 Simon Newton
 */

#include <ola/network/SelectServer.h>
#include <string>
#include "plugins/usbpro/UsbWidget.h"


#ifndef TOOLS_USBPRO_USBPRO_COMMON_H_
#define TOOLS_USBPRO_USBPRO_COMMON_H_

using std::string;
using ola::network::SelectServer;

/*
 * Abstract away the interface to the select server
 */
class MySelectServerAdaptor: public ola::plugin::usbpro::SelectServerAdaptor {
  public:
    explicit MySelectServerAdaptor(SelectServer *ss):
        m_ss(ss) {
    }

    bool AddSocket(ola::network::ConnectedSocket *socket,
                   bool delete_on_close = false) const {
      return m_ss->AddSocket(socket, delete_on_close);
    }
  private:
    SelectServer *m_ss;
};


int ConnectToWidget(const string &path);
#endif  // TOOLS_USBPRO_USBPRO_COMMON_H_
