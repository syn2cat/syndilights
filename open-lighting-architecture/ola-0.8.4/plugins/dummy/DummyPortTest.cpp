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
 * DummyPortTest.cpp
 * Test fixture for the E131PDU class
 * Copyright (C) 2005-2009 Simon Newton
 */

#include "plugins/dummy/DummyPort.h"  //  NOLINT, this has to be first
#include <cppunit/extensions/HelperMacros.h>
#include <string.h>
#include <string>
#include "ola/BaseTypes.h"
#include "ola/network/NetworkUtils.h"
#include "ola/rdm/RDMAPI.h"
#include "ola/rdm/RDMCommand.h"
#include "ola/rdm/UID.h"
#include "ola/rdm/UIDSet.h"

namespace ola {
namespace plugin {
namespace dummy {

using ola::network::HostToNetwork;
using ola::rdm::RDMGetRequest;
using ola::rdm::RDMGetResponse;
using ola::rdm::RDMRequest;
using ola::rdm::RDMResponse;
using ola::rdm::RDMSetRequest;
using ola::rdm::RDMSetResponse;
using ola::rdm::UID;
using ola::rdm::UIDSet;

class MockDummyPort: public DummyPort {
  public:
    MockDummyPort():
      DummyPort(NULL, 0),
      m_expected_response(NULL) {
    }

    bool HandleRDMResponse(const RDMResponse *response);
    void NewUIDList(const UIDSet &uids) {
      m_uid_set = uids;
    }

    const UIDSet &GetUIDSet() const { return m_uid_set; }
    void SetExpectedResponse(const RDMResponse *response);
  private:
    UIDSet m_uid_set;
    const RDMResponse *m_expected_response;
};


class DummyPortTest: public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(DummyPortTest);
  CPPUNIT_TEST(testRDMDiscovery);
  CPPUNIT_TEST(testUnknownPid);
  CPPUNIT_TEST(testSupportedParams);
  CPPUNIT_TEST(testDeviceInfo);
  CPPUNIT_TEST(testSoftwareVersion);
  CPPUNIT_TEST(testDmxAddress);
  CPPUNIT_TEST_SUITE_END();

  public:
    DummyPortTest():
      TestFixture(),
      m_expected_uid(0x7a70, 0xffffff00),
      m_test_source(1, 2) {
    }

    void testRDMDiscovery();
    void testUnknownPid();
    void testSupportedParams();
    void testDeviceInfo();
    void testSoftwareVersion();
    void testDmxAddress();

  private:
    UID m_expected_uid;
    UID m_test_source;
    MockDummyPort m_port;

    void checkSubDeviceOutOfRange(ola::rdm::rdm_pid pid);
    void checkMalformedRequest(ola::rdm::rdm_pid pid);
    void checkSetRequest(ola::rdm::rdm_pid pid);
    void checkNoBroadcastResponse(ola::rdm::rdm_pid pid);
};

CPPUNIT_TEST_SUITE_REGISTRATION(DummyPortTest);


bool MockDummyPort::HandleRDMResponse(const ola::rdm::RDMResponse *response) {
  CPPUNIT_ASSERT(m_expected_response);
  CPPUNIT_ASSERT(*m_expected_response == *response);
  delete response;
  delete m_expected_response;
  m_expected_response = NULL;
  return true;
}

void MockDummyPort::SetExpectedResponse(const RDMResponse *response) {
  CPPUNIT_ASSERT(!m_expected_response);
  m_expected_response = response;
}


/*
 * Check that RDM discovery works
 */
void DummyPortTest::testRDMDiscovery() {
  MockDummyPort port;
  const UIDSet &uids = port.GetUIDSet();
  CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), uids.Size());
  port.RunRDMDiscovery();
  CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), uids.Size());
  CPPUNIT_ASSERT(uids.Contains(m_expected_uid));
}


/*
 * Check that unknown pids fail
 */
void DummyPortTest::testUnknownPid() {
  UID source(1, 2);
  MockDummyPort port;

  RDMRequest *request = new RDMGetRequest(
      source,
      m_expected_uid,
      0,  // transaction #
      1,  // port id
      0,  // message count
      0,  // sub device
      ola::rdm::PID_COMMS_STATUS,  // param id
      NULL,  // data
      0);  // data length

  RDMResponse *response = NackWithReason(
      request,
      ola::rdm::NR_UNKNOWN_PID);

  port.SetExpectedResponse(response);
  CPPUNIT_ASSERT(port.HandleRDMRequest(request));
}


/*
 * Check that the supported params command work
 */
void DummyPortTest::testSupportedParams() {
  RDMRequest *request = new RDMGetRequest(
      m_test_source,
      m_expected_uid,
      0,  // transaction #
      1,  // port id
      0,  // message count
      0,  // sub device
      ola::rdm::PID_SUPPORTED_PARAMETERS,  // param id
      NULL,  // data
      0);  // data length

  uint16_t supported_params[] = {
    ola::rdm::PID_SUPPORTED_PARAMETERS,
    ola::rdm::PID_DEVICE_INFO,
    ola::rdm::PID_PRODUCT_DETAIL_ID_LIST,
    ola::rdm::PID_DEVICE_MODEL_DESCRIPTION,
    ola::rdm::PID_MANUFACTURER_LABEL,
    ola::rdm::PID_DEVICE_LABEL,
    ola::rdm::PID_SOFTWARE_VERSION_LABEL,
    ola::rdm::PID_DMX_START_ADDRESS
  };

  for (unsigned int i = 0; i < sizeof(supported_params) / 2; i++)
    supported_params[i] = HostToNetwork(supported_params[i]);

  RDMResponse *response = GetResponseWithData(
    request,
    reinterpret_cast<uint8_t*>(&supported_params),
    sizeof(supported_params));

  m_port.SetExpectedResponse(response);
  CPPUNIT_ASSERT(m_port.HandleRDMRequest(request));

  checkSubDeviceOutOfRange(ola::rdm::PID_SUPPORTED_PARAMETERS);
  checkMalformedRequest(ola::rdm::PID_SUPPORTED_PARAMETERS);
  checkSetRequest(ola::rdm::PID_SUPPORTED_PARAMETERS);
  checkNoBroadcastResponse(ola::rdm::PID_SUPPORTED_PARAMETERS);
}


/*
 * Check that the device info command works
 */
void DummyPortTest::testDeviceInfo() {
  RDMRequest *request = new RDMGetRequest(
      m_test_source,
      m_expected_uid,
      0,  // transaction #
      1,  // port id
      0,  // message count
      0,  // sub device
      ola::rdm::PID_DEVICE_INFO,  // param id
      NULL,  // data
      0);  // data length

  ola::rdm::DeviceDescriptor device_descriptor;
  device_descriptor.protocol_version_high = 1;
  device_descriptor.protocol_version_low = 0;
  device_descriptor.device_model = HostToNetwork(static_cast<uint16_t>(1));
  device_descriptor.product_category = HostToNetwork(
      static_cast<uint16_t>(ola::rdm::PRODUCT_CATEGORY_OTHER));
  device_descriptor.software_version = HostToNetwork(static_cast<uint32_t>(1));
  device_descriptor.dmx_footprint =
    HostToNetwork(static_cast<uint16_t>(10));
  device_descriptor.current_personality = 1;
  device_descriptor.personaility_count = 1;
  device_descriptor.dmx_start_address =
    HostToNetwork(static_cast<uint16_t>(1));
  device_descriptor.sub_device_count = 0;
  device_descriptor.sensor_count = 0;

  RDMResponse *response = GetResponseWithData(
    request,
    reinterpret_cast<uint8_t*>(&device_descriptor),
    sizeof(device_descriptor));

  m_port.SetExpectedResponse(response);
  CPPUNIT_ASSERT(m_port.HandleRDMRequest(request));

  checkSubDeviceOutOfRange(ola::rdm::PID_DEVICE_INFO);
  checkMalformedRequest(ola::rdm::PID_DEVICE_INFO);
  checkSetRequest(ola::rdm::PID_DEVICE_INFO);
  checkNoBroadcastResponse(ola::rdm::PID_DEVICE_INFO);
}


/*
 * Check that the software version command works
 */
void DummyPortTest::testSoftwareVersion() {
  RDMRequest *request = new RDMGetRequest(
      m_test_source,
      m_expected_uid,
      0,  // transaction #
      1,  // port id
      0,  // message count
      0,  // sub device
      ola::rdm::PID_SOFTWARE_VERSION_LABEL,  // param id
      NULL,  // data
      0);  // data length

  const string version = "Dummy Software Version";
  RDMResponse *response = GetResponseWithData(
    request,
    reinterpret_cast<const uint8_t*>(version.data()),
    version.size());

  m_port.SetExpectedResponse(response);
  CPPUNIT_ASSERT(m_port.HandleRDMRequest(request));

  checkSubDeviceOutOfRange(ola::rdm::PID_SOFTWARE_VERSION_LABEL);
  checkMalformedRequest(ola::rdm::PID_SOFTWARE_VERSION_LABEL);
  checkSetRequest(ola::rdm::PID_SOFTWARE_VERSION_LABEL);
  checkNoBroadcastResponse(ola::rdm::PID_SOFTWARE_VERSION_LABEL);
}


/*
 * Check that the dmx address command works
 */
void DummyPortTest::testDmxAddress() {
  RDMRequest *request = new RDMGetRequest(
      m_test_source,
      m_expected_uid,
      0,  // transaction #
      1,  // port id
      0,  // message count
      0,  // sub device
      ola::rdm::PID_DMX_START_ADDRESS,  // param id
      NULL,  // data
      0);  // data length

  uint16_t dmx_address = HostToNetwork(static_cast<uint16_t>(1));
  RDMResponse *response = GetResponseWithData(
    request,
    reinterpret_cast<const uint8_t*>(&dmx_address),
    sizeof(dmx_address));

  m_port.SetExpectedResponse(response);
  CPPUNIT_ASSERT(m_port.HandleRDMRequest(request));

  // now attempt to set it
  dmx_address = HostToNetwork(static_cast<uint16_t>(99));
  request = new RDMSetRequest(
      m_test_source,
      m_expected_uid,
      0,  // transaction #
      1,  // port id
      2,  // message count
      0,  // sub device
      ola::rdm::PID_DMX_START_ADDRESS,  // param id
      reinterpret_cast<const uint8_t*>(&dmx_address),
      sizeof(dmx_address));

  response = GetResponseWithData(request, NULL, 0);
  m_port.SetExpectedResponse(response);
  CPPUNIT_ASSERT(m_port.HandleRDMRequest(request));

  // now check it updated
  request = new RDMGetRequest(
      m_test_source,
      m_expected_uid,
      0,  // transaction #
      1,  // port id
      0,  // message count
      0,  // sub device
      ola::rdm::PID_DMX_START_ADDRESS,  // param id
      NULL,  // data
      0);  // data length

  dmx_address = HostToNetwork(static_cast<uint16_t>(99));
  response = GetResponseWithData(
    request,
    reinterpret_cast<const uint8_t*>(&dmx_address),
    sizeof(dmx_address));

  m_port.SetExpectedResponse(response);
  CPPUNIT_ASSERT(m_port.HandleRDMRequest(request));

  // check that broadcasting changes the address
  dmx_address = HostToNetwork(static_cast<uint16_t>(48));
  UID broadcast_uid = UID::AllManufactureDevices(OPEN_LIGHTING_ESTA_CODE);
  request = new RDMSetRequest(
      m_test_source,
      broadcast_uid,
      0,  // transaction #
      1,  // port id
      3,  // message count
      0,  // sub device
      ola::rdm::PID_DMX_START_ADDRESS,  // param id
      reinterpret_cast<const uint8_t*>(&dmx_address),
      sizeof(dmx_address));

  // no response expected
  CPPUNIT_ASSERT(m_port.HandleRDMRequest(request));

  // now check it updated
  request = new RDMGetRequest(
      m_test_source,
      m_expected_uid,
      0,  // transaction #
      1,  // port id
      0,  // message count
      0,  // sub device
      ola::rdm::PID_DMX_START_ADDRESS,  // param id
      NULL,  // data
      0);  // data length

  dmx_address = HostToNetwork(static_cast<uint16_t>(48));
  response = GetResponseWithData(
    request,
    reinterpret_cast<const uint8_t*>(&dmx_address),
    sizeof(dmx_address));

  m_port.SetExpectedResponse(response);
  CPPUNIT_ASSERT(m_port.HandleRDMRequest(request));

  checkSubDeviceOutOfRange(ola::rdm::PID_DMX_START_ADDRESS);
  checkMalformedRequest(ola::rdm::PID_DMX_START_ADDRESS);
  checkNoBroadcastResponse(ola::rdm::PID_DMX_START_ADDRESS);
}


void DummyPortTest::checkSubDeviceOutOfRange(ola::rdm::rdm_pid pid) {
  // a request with a non-0 subdevice
  RDMRequest *request = new RDMGetRequest(
      m_test_source,
      m_expected_uid,
      0,  // transaction #
      1,  // port id
      0,  // message count
      1,  // sub device
      pid,
      NULL,  // data
      0);  // data length

  RDMResponse *response = NackWithReason(
      request,
      ola::rdm::NR_SUB_DEVICE_OUT_OF_RANGE);
  m_port.SetExpectedResponse(response);
  CPPUNIT_ASSERT(m_port.HandleRDMRequest(request));
}


void DummyPortTest::checkMalformedRequest(ola::rdm::rdm_pid pid) {
  // a malformed request
  uint16_t bad_data = 0;
  RDMRequest *request = new RDMGetRequest(
      m_test_source,
      m_expected_uid,
      0,  // transaction #
      1,  // port id
      0,  // message count
      0,  // sub device
      pid,
      reinterpret_cast<uint8_t*>(&bad_data),  // data
      sizeof(bad_data));  // data length

  RDMResponse *response = NackWithReason(
      request,
      ola::rdm::NR_FORMAT_ERROR);
  m_port.SetExpectedResponse(response);
  CPPUNIT_ASSERT(m_port.HandleRDMRequest(request));
}


void DummyPortTest::checkSetRequest(ola::rdm::rdm_pid pid) {
  // a set request
  RDMRequest *request = new RDMSetRequest(
      m_test_source,
      m_expected_uid,
      0,  // transaction #
      1,  // port id
      0,  // message count
      0,  // sub device
      pid,
      NULL,  // data
      0);  // data length

  RDMResponse *response = NackWithReason(
      request,
      ola::rdm::NR_UNSUPPORTED_COMMAND_CLASS);
  m_port.SetExpectedResponse(response);
  CPPUNIT_ASSERT(m_port.HandleRDMRequest(request));
}


void DummyPortTest::checkNoBroadcastResponse(ola::rdm::rdm_pid pid) {
  // a broadcast request
  UID broadcast_uid = UID::AllDevices();
  RDMRequest *request = new RDMGetRequest(
      m_test_source,
      broadcast_uid,
      0,  // transaction #
      1,  // port id
      0,  // message count
      0,  // sub device
      pid,  // param id
      NULL,  // data
      0);  // data length

  // we don't expect any response to this
  CPPUNIT_ASSERT(m_port.HandleRDMRequest(request));

  broadcast_uid = UID::AllManufactureDevices(OPEN_LIGHTING_ESTA_CODE);
  request = new RDMGetRequest(
      m_test_source,
      broadcast_uid,
      0,  // transaction #
      1,  // port id
      0,  // message count
      0,  // sub device
      pid,  // param id
      NULL,  // data
      0);  // data length

  // we don't expect any response to this
  CPPUNIT_ASSERT(m_port.HandleRDMRequest(request));
}
}  // dummy
}  // ola
}  // plugin
