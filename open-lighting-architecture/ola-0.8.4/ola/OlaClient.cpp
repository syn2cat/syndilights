/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * OlaClient.cpp
 * Implementation of OlaClient
 * Copyright (C) 2005-2008 Simon Newton
 */

#include <string>
#include "ola/OlaClient.h"
#include "ola/OlaClientCore.h"

namespace ola {

using ola::network::ConnectedSocket;

OlaClient::OlaClient(ConnectedSocket *socket):
  m_observer(NULL) {
  m_core = new OlaClientCore(socket);
}

OlaClient::~OlaClient() {
  delete m_core;
}


/*
 * Setup this client
 *
 * @returns true on success, false on failure
 */
bool OlaClient::Setup() {
  return m_core->Setup();
}


/*
 * Close the ola connection.
 *
 * @return true on sucess, false on failure
 */
bool OlaClient::Stop() {
  return m_core->Stop();
}


/*
 * Set the OlaClientObserver object
 *
 * @params observer the OlaClientObserver object to be used for the callbacks.
 */
void OlaClient::SetObserver(OlaClientObserver *observer) {
  m_observer = observer;
  m_core->SetDmxCallback(NewCallback(observer, &OlaClientObserver::NewDmx));
}


/*
 * Fetch info about available plugins. This results in a call to
 *   observer->Plugins(...)
 *  when the request returns.
 * @returns true if the request succeeded, false otherwise.
 */
bool OlaClient::FetchPluginList() {
  if (m_observer)
    return m_core->FetchPluginList(
        NewSingleCallback(m_observer, &OlaClientObserver::Plugins));
  else
    return m_core->FetchPluginList(NULL);
}


/*
 * Fetch the description for a plugin. This calls
 *   observer->PluginDescription(...)
 *  when the request returns.
 * @returns true if the request succeeded, false otherwise.
 */
bool OlaClient::FetchPluginDescription(ola_plugin_id plugin_id) {
  if (m_observer)
    return m_core->FetchPluginDescription(
        plugin_id,
        NewSingleCallback(m_observer,
                          &OlaClientObserver::PluginDescription,
                          plugin_id));
  else
    return m_core->FetchPluginDescription(plugin_id, NULL);
}


/*
 * Write some dmx data.
 * @param universe   universe to send to
 * @param data  a DmxBuffer with the data
 * @return true on success, false on failure
 */
bool OlaClient::SendDmx(unsigned int universe, const DmxBuffer &data) {
  if (m_observer)
    return m_core->SendDmx(
        universe,
        data,
        NewSingleCallback(m_observer,
                          &OlaClientObserver::SendDmxComplete,
                          universe));
  else
    return m_core->SendDmx(universe, data, NULL);
}


/*
 * Read dmx data. This results in a call to
 *  observer->NewDmx()
 * when the request returns.
 *
 * @param universe the universe id to get data for
 * @return true on success, false on failure
 */
bool OlaClient::FetchDmx(unsigned int universe) {
  if (m_observer)
    return m_core->FetchDmx(
        universe,
        NewSingleCallback(m_observer, &OlaClientObserver::NewDmx, universe));
  else
    return m_core->FetchDmx(universe, NULL);
}


/*
 * Fetch the UID list for a universe
 * @param universe the universe id to get data for
 * @return true on success, false on failure
 */
bool OlaClient::FetchUIDList(unsigned int universe) {
  if (m_observer)
    return m_core->FetchUIDList(
        universe,
        NewSingleCallback(m_observer, &OlaClientObserver::UIDList, universe));
  else
    return m_core->FetchUIDList(universe, NULL);
}


/*
 * Force RDM discovery for a universe
 * @param universe the universe id to run discovery on
 * @return true on success, false on failure
 */
bool OlaClient::ForceDiscovery(unsigned int universe) {
  if (m_observer)
    return m_core->ForceDiscovery(
        universe,
        NewSingleCallback(m_observer,
                          &OlaClientObserver::ForceRDMDiscoveryComplete,
                          universe));
  else
    return m_core->ForceDiscovery(universe, NULL);
}


/*
 * Set this clients Source UID
 */
bool OlaClient::SetSourceUID(const UID &uid) {
  if (m_observer)
    return m_core->SetSourceUID(
        uid,
        NewSingleCallback(m_observer,
                          &OlaClientObserver::SetSourceUIDComplete));
  else
    return m_core->SetSourceUID(uid, NULL);
}


/*
 * Send an RDM Get Command
 * @param callback the Callback to invoke when this completes
 * @param universe the universe to send the command on
 * @param uid the UID to send the command to
 * @param sub_device the sub device index
 * @param pid the PID to address
 * @param data the optional data to send
 * @param data_length the length of the data
 * @return true on success, false on failure
 */
bool OlaClient::RDMGet(rdm_callback *callback,
            unsigned int universe,
            const UID &uid,
            uint16_t sub_device,
            uint16_t pid,
            const uint8_t *data,
            unsigned int data_length) {
  return m_core->RDMGet(callback,
                        universe,
                        uid,
                        sub_device,
                        pid,
                        data,
                        data_length);
}


/*
 * Send an RDM Set Command
 * @param callback the Callback to invoke when this completes
 * @param universe the universe to send the command on
 * @param uid the UID to send the command to
 * @param sub_device the sub device index
 * @param pid the PID to address
 * @param data the optional data to send
 * @param data_length the length of the data
 * @return true on success, false on failure
 */
bool OlaClient::RDMSet(rdm_callback *callback,
                       unsigned int universe,
                       const UID &uid,
                       uint16_t sub_device,
                       uint16_t pid,
                       const uint8_t *data,
                       unsigned int data_length) {
  return m_core->RDMSet(callback,
                        universe,
                        uid,
                        sub_device,
                        pid,
                        data,
                        data_length);
}


/*
 * Request a listing of what devices are attached. This results in a call to
 *   observer->Devices()
 * when the request returns.
 * @param filter only fetch devices that belong to this plugin
 * @return true on success, false on failure
 */
bool OlaClient::FetchDeviceInfo(ola_plugin_id filter) {
  if (m_observer)
    return m_core->FetchDeviceInfo(
        filter,
        NewSingleCallback(m_observer, &OlaClientObserver::Devices));
  else
    return m_core->FetchDeviceInfo(filter, NULL);
}


/*
 * Request information about active universes. This results in a call to
 *   observer->Universes()
 * when the request returns.
 * @return true on success, false on failure
 */
bool OlaClient::FetchUniverseInfo() {
  if (m_observer)
    return m_core->FetchUniverseList(
        NewSingleCallback(m_observer, &OlaClientObserver::Universes));
  else
    return m_core->FetchUniverseList(NULL);
}


/*
 * Set the name of a universe.
 * @return true on success, false on failure
 */
bool OlaClient::SetUniverseName(unsigned int universe, const string &name) {
  if (m_observer)
    return m_core->SetUniverseName(
        universe,
        name,
        NewSingleCallback(m_observer,
                          &OlaClientObserver::UniverseNameComplete,
                          universe));
  else
    return m_core->SetUniverseName(universe, name, NULL);
}


/*
 * Set the merge mode of a universe
 *
 * @return true on success, false on failure
 */
bool OlaClient::SetUniverseMergeMode(unsigned int universe,
                                     OlaUniverse::merge_mode mode) {
  if (m_observer)
    return m_core->SetUniverseMergeMode(
        universe,
        mode,
        NewSingleCallback(m_observer,
                          &OlaClientObserver::UniverseMergeModeComplete,
                          universe));
  else
    return m_core->SetUniverseMergeMode(universe, mode, NULL);
}


/*
 * Register our interest in a universe. This results in calls to
 *   observer->NewDmx()
 * whenever the dmx values change.
 *
 * @param uni  the universe id
 * @param action REGISTER or UNREGISTER
 * @return true on success, false on failure
 */
bool OlaClient::RegisterUniverse(unsigned int universe,
                                 ola::RegisterAction register_action) {
  if (m_observer)
    return m_core->RegisterUniverse(
        universe,
        register_action,
        NewSingleCallback(m_observer,
                          &OlaClientObserver::RegistrationComplete,
                          universe));
  else
    return m_core->RegisterUniverse(universe, register_action, NULL);
}


/*
 * (Un)Patch a port to a universe
 * @param dev     the device id
 * @param port    the port id
 * @param port_direction the direction of the port
 * @param action  OlaClient::PATCH or OlaClient::UNPATCH
 * @param uni    universe id
 * @return true on success, false on failure
 */
bool OlaClient::Patch(unsigned int device_alias,
                      unsigned int port_id,
                      PortDirection port_direction,
                      ola::PatchAction patch_action,
                      unsigned int universe) {
  if (m_observer)
    return m_core->Patch(
        device_alias,
        port_id,
        port_direction,
        patch_action,
        universe,
        NewSingleCallback(m_observer,
                          &OlaClientObserver::PatchComplete,
                          device_alias,
                          port_id,
                          port_direction));
  else
    return m_core->Patch(
        device_alias,
        port_id,
        port_direction,
        patch_action,
        universe,
        NULL);
}


/*
 * Set the priority for a port to inherit mode
 * @param dev the device id
 * @param port the port id
 * @param port_direction the direction of the port
 */
bool OlaClient::SetPortPriorityInherit(unsigned int device_alias,
                                       unsigned int port,
                                       PortDirection port_direction) {
  if (m_observer)
    return m_core->SetPortPriorityInherit(
        device_alias,
        port,
        port_direction,
        NewSingleCallback(m_observer,
                          &OlaClientObserver::SetPortPriorityComplete,
                          device_alias,
                          port,
                          port_direction));
  else
    return m_core->SetPortPriorityInherit(
        device_alias,
        port,
        port_direction,
        NULL);
}


/*
 * Set the priority for a port to override mode
 * @param dev the device id
 * @param port the port id
 * @param port_direction the direction of the port
 * @param value the port priority value
 */
bool OlaClient::SetPortPriorityOverride(unsigned int device_alias,
                                        unsigned int port,
                                        PortDirection port_direction,
                                        uint8_t value) {
  if (m_observer)
    return m_core->SetPortPriorityOverride(
        device_alias,
        port,
        port_direction,
        value,
        NewSingleCallback(m_observer,
                          &OlaClientObserver::SetPortPriorityComplete,
                          device_alias,
                          port,
                          port_direction));
  else
    return m_core->SetPortPriorityOverride(
        device_alias,
        port,
        port_direction,
        value,
        NULL);
}


/*
 * Sends a device config request
 * @param device_alias the device_alias
 * @param msg  the request message
 * @return true on success, false on failure
 */
bool OlaClient::ConfigureDevice(unsigned int device_alias, const string &msg) {
  if (m_observer)
    return m_core->ConfigureDevice(
        device_alias,
        msg,
        NewSingleCallback(m_observer,
                          &OlaClientObserver::DeviceConfig,
                          device_alias));
  else
    return m_core->ConfigureDevice(device_alias, msg, NULL);
}
}  // ola
