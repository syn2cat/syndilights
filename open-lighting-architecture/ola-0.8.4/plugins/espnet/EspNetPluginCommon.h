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
 *
 * common.h
 * Constants for the esp plugin
 * Copyright (C) 2005  Simon Newton
 */

#ifndef PLUGINS_ESPNET_ESPNETPLUGINCOMMON_H_
#define PLUGINS_ESPNET_ESPNETPLUGINCOMMON_H_

namespace ola {
namespace plugin {
namespace espnet {

// modify this to increase the number of ports in the plugin
enum { PORTS_PER_DEVICE = 5 };
}  // espnet
}  // plugin
}  // ola
#endif  // PLUGINS_ESPNET_ESPNETPLUGINCOMMON_H_
