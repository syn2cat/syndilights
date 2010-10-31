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
 * InterfacePicker.h
 * Choose an interface to listen on
 * Copyright (C) 2005-2008 Simon Newton
 */

#ifndef INCLUDE_OLA_NETWORK_INTERFACEPICKER_H_
#define INCLUDE_OLA_NETWORK_INTERFACEPICKER_H_

#ifdef WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

#include <ola/network/Interface.h>
#include <string>
#include <vector>

namespace ola {
namespace network {


/*
 * Chooses an interface
 */
class InterfacePicker {
  public:
    InterfacePicker() {}
    virtual ~InterfacePicker() {}

    // stupid windows, 'interface' seems to be a struct so we use iface here.
    bool ChooseInterface(Interface *iface,
                         const std::string &preferred_ip) const;

    virtual std::vector<Interface> GetInterfaces() const = 0;

    static InterfacePicker *NewPicker();
};
}  // network
}  // ola
#endif  // INCLUDE_OLA_NETWORK_INTERFACEPICKER_H_

