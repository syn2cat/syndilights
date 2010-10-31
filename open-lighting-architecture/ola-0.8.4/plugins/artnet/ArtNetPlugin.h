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
 * ArtNetPlugin.h
 * Interface for the ArtNet plugin class
 * Copyright (C) 2005-2009 Simon Newton
 */

#ifndef PLUGINS_ARTNET_ARTNETPLUGIN_H_
#define PLUGINS_ARTNET_ARTNETPLUGIN_H_

#include <string>
#include "olad/Plugin.h"
#include "ola/plugin_id.h"

namespace ola {
namespace plugin {
namespace artnet {

using ola::Plugin;
using ola::PluginAdaptor;
using std::string;

class ArtNetDevice;

class ArtNetPlugin : public Plugin {
  public:
    explicit ArtNetPlugin(const PluginAdaptor *plugin_adaptor):
      Plugin(plugin_adaptor),
      m_device(NULL) {}

    ~ArtNetPlugin() {}

    string Name() const { return PLUGIN_NAME; }
    ola_plugin_id Id() const { return OLA_PLUGIN_ARTNET; }
    string Description() const;
    string PluginPrefix() const { return PLUGIN_PREFIX; }

  private:
    bool StartHook();
    bool StopHook();
    bool SetDefaultPreferences();

    ArtNetDevice *m_device;  // only have one device

    static const char ARTNET_SUBNET[];
    static const char ARTNET_LONG_NAME[];
    static const char ARTNET_SHORT_NAME[];
    static const char PLUGIN_NAME[];
    static const char PLUGIN_PREFIX[];
};
}  // artnet
}  // plugin
}  // ola
#endif  // PLUGINS_ARTNET_ARTNETPLUGIN_H_
