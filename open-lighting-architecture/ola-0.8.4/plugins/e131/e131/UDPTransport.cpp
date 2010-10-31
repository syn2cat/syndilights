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
 * UDPTransport.h
 * The UDPTransport class
 * Copyright (C) 2007 Simon Newton
 */

#include "plugins/e131/e131/E131Includes.h"  //  NOLINT, this has to be first
#include <string.h>
#include <string>

#include "ola/Closure.h"
#include "ola/Logging.h"
#include "ola/network/NetworkUtils.h"
#include "plugins/e131/e131/UDPTransport.h"

namespace ola {
namespace plugin {
namespace e131 {

using ola::network::HostToNetwork;

const char UDPTransport::ACN_PACKET_ID[] = "ASC-E1.17\0\0\0";

/*
 * Clean up
 */
UDPTransport::~UDPTransport() {
  m_socket.Close();
  if (m_send_buffer)
    delete[] m_send_buffer;
  if (m_recv_buffer)
    delete[] m_recv_buffer;
}


/*
 * Setup the UDP Transport
 */
bool UDPTransport::Init(const ola::network::Interface &interface) {
  if (!m_socket.Init())
    return false;

  if (!m_socket.Bind(m_port))
    return false;

  if (!m_socket.EnableBroadcast())
    return false;

  m_socket.SetOnData(NewClosure(this, &UDPTransport::Receive));

  if (!m_send_buffer) {
    m_send_buffer = new uint8_t[MAX_DATAGRAM_SIZE];
    memset(m_send_buffer, 0 , DATA_OFFSET);
    uint16_t *ptr = reinterpret_cast<uint16_t*>(m_send_buffer);
    *ptr++ = HostToNetwork(PREAMBLE_SIZE);
    *ptr = HostToNetwork(POSTABLE_SIZE);
    strncpy(reinterpret_cast<char*>(m_send_buffer + PREAMBLE_OFFSET),
            ACN_PACKET_ID,
            strlen(ACN_PACKET_ID));
  }

  if (!m_recv_buffer)
    m_recv_buffer = new uint8_t[MAX_DATAGRAM_SIZE];

  m_interface = interface;
  return true;
}


/*
 * Send a block of PDU messages, this may send separate packets if the size of
 * the block is greater than the MAX_DATAGRAM_SIZE.
 * @param pdu_block the block of pdus to send
 */
bool UDPTransport::Send(const PDUBlock<PDU> &pdu_block,
                        const struct sockaddr_in &destination) {
  if (!m_send_buffer) {
    OLA_WARN << "Send called the transport hasn't been initialized";
    return false;
  }

  unsigned int size = MAX_DATAGRAM_SIZE - DATA_OFFSET;
  if (!pdu_block.Pack(m_send_buffer + DATA_OFFSET, size))
    return false;

  return m_socket.SendTo(m_send_buffer, DATA_OFFSET + size, destination);
}


/*
 * Called when new data arrives.
 */
void UDPTransport::Receive() {
  if (!m_recv_buffer) {
    OLA_WARN << "Receive called the transport hasn't been initialized";
    return;
  }

  ssize_t size = MAX_DATAGRAM_SIZE;
  if (!m_socket.RecvFrom(m_recv_buffer, &size))
    return;

  if (size < (ssize_t) DATA_OFFSET) {
    OLA_WARN << "short ACN frame, discarding";
    return;
  }

  if (memcmp(m_recv_buffer, m_send_buffer, DATA_OFFSET)) {
    OLA_WARN << "ACN header is bad, discarding";
    return;
  }

  HeaderSet header_set;
  m_inflator->InflatePDUBlock(header_set,
                              m_recv_buffer + DATA_OFFSET,
                              static_cast<unsigned int>(size - DATA_OFFSET));
  return;
}


bool UDPTransport::JoinMulticast(const struct in_addr &group) {
  return m_socket.JoinMulticast(m_interface.ip_address, group);
}


bool UDPTransport::LeaveMulticast(const struct in_addr &group) {
  return m_socket.LeaveMulticast(m_interface.ip_address, group);
}
}  // e131
}  // plugin
}  // ola
