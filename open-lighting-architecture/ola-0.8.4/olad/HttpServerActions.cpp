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
 * HttpServerActions.cpp
 * The actions the HTTP server uses to interact with the client.
 * Copyright (C) 2010 Simon Newton
 */

#include <string>
#include <vector>

#include "ola/ActionQueue.h"
#include "ola/Callback.h"
#include "ola/Closure.h"
#include "ola/Logging.h"
#include "olad/HttpServerActions.h"

namespace ola {

using std::string;


void BaseHttpAction::RequestComplete(bool failure) {
  m_failed = failure;
  m_on_done->Run();
}


void BaseHttpAction::Perform(SingleUseClosure<void> *on_done) {
  m_on_done = on_done;
  if (!DoAction())
    RequestComplete(true);
}

void BaseHttpAction::CallbackComplete(const string &error) {
  RequestComplete(!error.empty());
}


bool SetNameAction::DoAction() {
  return m_client->SetUniverseName(
    m_universe,
    m_name,
    NewSingleCallback(reinterpret_cast<BaseHttpAction*>(this),
                      &SetNameAction::CallbackComplete));
}



bool SetMergeModeAction::DoAction() {
  return m_client->SetUniverseMergeMode(
    m_universe,
    m_merge_mode,
    NewSingleCallback(reinterpret_cast<BaseHttpAction*>(this),
                      &SetNameAction::CallbackComplete));
}


bool PatchPortAction::DoAction() {
  return m_client->Patch(
    m_device_alias,
    m_port,
    m_direction,
    m_action,
    m_universe,
    NewSingleCallback(reinterpret_cast<BaseHttpAction*>(this),
                      &SetNameAction::CallbackComplete));
}


bool PortPriorityInheritAction::DoAction() {
  return m_client->SetPortPriorityInherit(
    m_device_alias,
    m_port,
    m_direction,
    NewSingleCallback(reinterpret_cast<BaseHttpAction*>(this),
                      &SetNameAction::CallbackComplete));
}


bool PortPriorityOverrideAction::DoAction() {
  return m_client->SetPortPriorityOverride(
    m_device_alias,
    m_port,
    m_direction,
    m_override_value,
    NewSingleCallback(reinterpret_cast<BaseHttpAction*>(this),
                      &SetNameAction::CallbackComplete));
}
}  // ola
