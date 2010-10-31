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
 * DlOpenPluginLoader.h
 * Interface for the DlOpenPluginLoader class
 * Copyright (C) 2005-2008 Simon Newton
 */

#ifndef OLAD_DLOPENPLUGINLOADER_H_
#define OLAD_DLOPENPLUGINLOADER_H_

#include <ltdl.h>
#include <string>
#include <vector>
#include <set>

#include "olad/PluginLoader.h"

namespace ola {

using std::set;
using std::string;
using std::vector;

class AbstractPlugin;

class DlOpenPluginLoader: public PluginLoader {
  public:
    explicit DlOpenPluginLoader(const string &dirname):
      m_dirname(dirname),
      m_dl_active(false) {}
    ~DlOpenPluginLoader() { UnloadPlugins(); }

    std::vector<AbstractPlugin*> LoadPlugins();
    void UnloadPlugins();

  private:
    DlOpenPluginLoader(const DlOpenPluginLoader&);
    DlOpenPluginLoader operator=(const DlOpenPluginLoader&);

    set<string> FindPlugins(const string &path);
    AbstractPlugin *LoadPlugin(const string &path);

    string m_dirname;
    bool m_dl_active;
    std::vector<lt_dlhandle> m_plugin_handles;
};
}  // ola
#endif  // OLAD_DLOPENPLUGINLOADER_H_
