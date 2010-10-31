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
 * RDMAPITest.cpp
 * Test fixture for the RDM API class
 * Copyright (C) 2005-2010 Simon Newton
 */

#include <cppunit/extensions/HelperMacros.h>
#include <string.h>
#include <deque>
#include <string>
#include <vector>

#include "ola/network/NetworkUtils.h"
#include "ola/rdm/UID.h"
#include "ola/rdm/RDMAPI.h"
#include "ola/rdm/RDMAPIImplInterface.h"


using ola::NewSingleCallback;
using ola::network::HostToNetwork;
using ola::rdm::RDMAPI;
using ola::rdm::RDMAPIImplResponseStatus;
using ola::rdm::ResponseStatus;
using ola::rdm::UID;
using std::deque;
using std::string;
using std::vector;


class ExpectedResult {
  public:
    unsigned int universe;
    const UID uid;
    uint16_t sub_device;
    uint16_t pid;
    const string return_data;
    const uint8_t *data;
    unsigned int data_length;

    ExpectedResult(unsigned int universe,
                   const UID &uid,
                   uint16_t sub_device,
                   uint16_t pid,
                   const string &return_data,
                   const uint8_t *data_arg = NULL,
                   unsigned int data_length = 0):
      universe(universe),
      uid(uid),
      sub_device(sub_device),
      pid(pid),
      return_data(return_data),
      data(NULL),
      data_length(data_length) {
    if (data_arg) {
      uint8_t *d = new uint8_t[data_length];
      memcpy(d, data_arg, data_length);
      data = d;
    }
  }

  ~ExpectedResult() {
    if (data)
      delete[] data;
  }
};

class MockRDMAPIImpl: public ola::rdm::RDMAPIImplInterface {
  public:
    bool RDMGet(rdm_callback *callback,
                unsigned int universe,
                const UID &uid,
                uint16_t sub_device,
                uint16_t pid,
                const uint8_t *data = NULL,
                unsigned int data_length = 0) {
      CPPUNIT_ASSERT(m_get_expected.size());
      const ExpectedResult *result = m_get_expected.front();
      CPPUNIT_ASSERT_EQUAL(result->universe, universe);
      CPPUNIT_ASSERT_EQUAL(result->uid, uid);
      CPPUNIT_ASSERT_EQUAL(result->sub_device, sub_device);
      CPPUNIT_ASSERT_EQUAL(result->pid, pid);

      (void) data;
      (void) data_length;

      RDMAPIImplResponseStatus status;
      status.was_broadcast = uid.IsBroadcast();
      status.response_type = ola::rdm::ACK;
      status.message_count = 0;
      callback->Run(status, result->return_data);

      delete result;
      m_get_expected.pop_front();
      return true;
    }

    bool RDMSet(rdm_callback *callback,
                unsigned int universe,
                const UID &uid,
                uint16_t sub_device,
                uint16_t pid,
                const uint8_t *data = NULL,
                unsigned int data_length = 0) {
      CPPUNIT_ASSERT(m_set_expected.size());
      const ExpectedResult *result = m_set_expected.front();
      CPPUNIT_ASSERT_EQUAL(result->universe, universe);
      CPPUNIT_ASSERT_EQUAL(result->uid, uid);
      CPPUNIT_ASSERT_EQUAL(result->sub_device, sub_device);
      CPPUNIT_ASSERT_EQUAL(result->pid, pid);

      if (result->data) {
        CPPUNIT_ASSERT_EQUAL(result->data_length, data_length);
        CPPUNIT_ASSERT(0 == memcmp(result->data, data, data_length));
      }

      RDMAPIImplResponseStatus status;
      status.was_broadcast = uid.IsBroadcast();
      status.response_type = ola::rdm::ACK;
      status.message_count = 0;
      callback->Run(status, result->return_data);

      delete result;
      m_set_expected.pop_front();
      return true;
    }

    void AddExpectedGet(
                        // const RDMAPIImplResponseStatus &status,
                        const string &return_data,
                        unsigned int universe,
                        const UID &uid,
                        uint16_t sub_device,
                        uint16_t pid,
                        const uint8_t *data = NULL,
                        unsigned int data_length = 0) {
      ExpectedResult *expected_result = new ExpectedResult(universe,
                                                           uid,
                                                           sub_device,
                                                           pid,
                                                           return_data,
                                                           data,
                                                           data_length);
      m_get_expected.push_back(expected_result);
    }

    void AddExpectedSet(unsigned int universe,
                        const UID &uid,
                        uint16_t sub_device,
                        uint16_t pid,
                        const uint8_t *data = NULL,
                        unsigned int data_length = 0) {
      string s;
      ExpectedResult *expected_result = new ExpectedResult(universe,
                                                           uid,
                                                           sub_device,
                                                           pid,
                                                           s,
                                                           data,
                                                           data_length);
      m_set_expected.push_back(expected_result);
    }

    void Verify() {
      CPPUNIT_ASSERT(!m_get_expected.size());
      CPPUNIT_ASSERT(!m_set_expected.size());
    }

  private:
    deque<ExpectedResult*> m_get_expected;
    deque<ExpectedResult*> m_set_expected;
};


class RDMAPITest: public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(RDMAPITest);
  CPPUNIT_TEST(testProxyCommands);
  CPPUNIT_TEST(testNetworkCommands);
  CPPUNIT_TEST(testRDMInformation);
  CPPUNIT_TEST(testProductInformation);
  CPPUNIT_TEST(testDmxSetup);
  CPPUNIT_TEST_SUITE_END();

  public:
    RDMAPITest():
      m_api(&m_impl),
      m_uid(1, 2),
      m_bcast_uid(UID::AllDevices()),
      m_group_uid(UID::AllManufactureDevices(52)),
      m_test_uid1(4, 5),
      m_test_uid2(7, 9) {
    }
    void testProxyCommands();
    void testNetworkCommands();
    void testRDMInformation();
    void testProductInformation();
    void testDmxSetup();
    void setUp() {}
    void tearDown();

  private:
    static const unsigned int UNIVERSE = 1;
    static const char BROADCAST_ERROR[];
    static const char DEVICE_RANGE_ERROR[];
    static const char DEVICE_RANGE_BCAST_ERROR[];
    static const char TEST_DESCRIPTION[];
    MockRDMAPIImpl m_impl;
    RDMAPI m_api;
    UID m_uid;
    UID m_bcast_uid;
    UID m_group_uid;
    UID m_test_uid1;
    UID m_test_uid2;

    // check that a RDM call failed because we tried to send to the bcast UID
    void CheckForBroadcastError(string *error) {
      CPPUNIT_ASSERT_EQUAL(string(BROADCAST_ERROR), *error);
      error->clear();
    }

    // check that a RDM call failed because we tried to send to all devices
    void CheckForDeviceRangeError(string *error) {
      CPPUNIT_ASSERT_EQUAL(string(DEVICE_RANGE_ERROR), *error);
      error->clear();
    }

    void CheckForDeviceRangeBcastError(string *error) {
      CPPUNIT_ASSERT_EQUAL(string(DEVICE_RANGE_BCAST_ERROR), *error);
      error->clear();
    }

    // check that a RDM command was successful
    void CheckResponseStatus(const ResponseStatus &status) {
      CPPUNIT_ASSERT_EQUAL(ola::rdm::ResponseStatus::VALID_RESPONSE,
                           status.ResponseType());
    }

    // check that a RDM command was successful and broadcast
    void CheckWasBroadcast(const ResponseStatus &status) {
      CPPUNIT_ASSERT_EQUAL(ola::rdm::ResponseStatus::BROADCAST_REQUEST,
                           status.ResponseType());
    }

    void CheckProxiedDeviceCount(const ResponseStatus &status,
                                 uint16_t count,
                                 bool changed) {
      CheckResponseStatus(status);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(2), count);
      CPPUNIT_ASSERT_EQUAL(false, changed);
    }

    void CheckProxiedDevices(const ResponseStatus &status,
                             const vector<UID> &devices) {
      CheckResponseStatus(status);
      CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), devices.size());
      CPPUNIT_ASSERT_EQUAL(m_test_uid1, devices[0]);
      CPPUNIT_ASSERT_EQUAL(m_test_uid2, devices[1]);
    }

    void CheckCommsStatus(const ResponseStatus &status,
                          uint16_t short_message,
                          uint16_t length_mismatch,
                          uint16_t checksum_fail) {
      CheckResponseStatus(status);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(14), short_message);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(187), length_mismatch);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(92), checksum_fail);
    }

    void CheckLabel(const ResponseStatus &status,
                                  const string &description) {
      CheckResponseStatus(status);
      CPPUNIT_ASSERT_EQUAL(string(TEST_DESCRIPTION), description);
    }

    void CheckSupportedParams(const ResponseStatus &status,
                              const vector<uint16_t> &params) {
      CheckResponseStatus(status);
      CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), params.size());
      // params are sorted
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(0x00aa), params[0]);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(0x1234), params[1]);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(0xabcd), params[2]);
    }

    void CheckParameterDescription(
        const ResponseStatus &status,
        const ola::rdm::ParameterDescriptor &description) {
      CheckResponseStatus(status);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(0x1234), description.pid);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(10), description.pdl_size);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(ola::rdm::DS_UNSIGNED_DWORD),
                           description.data_type);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(ola::rdm::CC_GET),
                           description.command_class);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(ola::rdm::UNITS_METERS),
                           description.unit);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(ola::rdm::PREFIX_KILO),
                           description.prefix);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint32_t>(0), description.min_value);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint32_t>(200000),
                           description.max_value);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint32_t>(1000),
                           description.default_value);
      CPPUNIT_ASSERT_EQUAL(string(TEST_DESCRIPTION).size(),
                           description.description.size());
      CPPUNIT_ASSERT_EQUAL(string(TEST_DESCRIPTION), description.description);
    }

    void CheckMalformedParameterDescription(
        const ResponseStatus &status,
        const ola::rdm::ParameterDescriptor &description) {
      CPPUNIT_ASSERT_EQUAL(ola::rdm::ResponseStatus::MALFORMED_RESPONSE,
                           status.ResponseType());
      (void) description;
    }

    void CheckDeviceInfo(const ResponseStatus &status,
                         const ola::rdm::DeviceDescriptor &descriptor) {
      CheckResponseStatus(status);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(1),
                           descriptor.protocol_version_high);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(0),
                           descriptor.protocol_version_low);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(2),
                           descriptor.device_model);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(3),
                           descriptor.product_category);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint32_t>(0x12345678),
                           descriptor.software_version);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(400),
                           descriptor.dmx_footprint);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(1),
                           descriptor.current_personality);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(2),
                           descriptor.personaility_count);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(12),
                           descriptor.dmx_start_address);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(10),
                           descriptor.sub_device_count);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(4),
                           descriptor.sensor_count);
    }

    void CheckProductDetailList(const ResponseStatus &status,
                                const vector<uint16_t> &params) {
      CheckResponseStatus(status);
      CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), params.size());
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(0x5678), params[0]);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(0xfedc), params[1]);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(0xaa00), params[2]);
    }

    void CheckDMXStartAddress(const ResponseStatus &status,
                              uint16_t start_address) {
      CheckResponseStatus(status);
      CPPUNIT_ASSERT_EQUAL(static_cast<uint16_t>(44), start_address);
    }
};

const char RDMAPITest::BROADCAST_ERROR[] = "Cannot send to broadcast address";
const char RDMAPITest::DEVICE_RANGE_ERROR[] = "Sub device must be <= 0x0200";
const char RDMAPITest::DEVICE_RANGE_BCAST_ERROR[] =
  "Sub device must be <= 0x0200 or 0xffff";
const char RDMAPITest::TEST_DESCRIPTION[] = "This is a description";

void RDMAPITest::tearDown() {
  m_impl.Verify();
}

CPPUNIT_TEST_SUITE_REGISTRATION(RDMAPITest);


/*
 * Test the proxied commands work.
 */
void RDMAPITest::testProxyCommands() {
  string error;
  // get proxied device count
  CPPUNIT_ASSERT(!m_api.GetProxiedDeviceCount(
    UNIVERSE,
    m_bcast_uid,
    NewSingleCallback(this, &RDMAPITest::CheckProxiedDeviceCount),
    &error));
  CheckForBroadcastError(&error);
  CPPUNIT_ASSERT(!m_api.GetProxiedDeviceCount(
    UNIVERSE,
    m_group_uid,
    NewSingleCallback(this, &RDMAPITest::CheckProxiedDeviceCount),
    &error));
  CheckForBroadcastError(&error);

  struct {
    uint16_t count;
    uint8_t changed;
  } count_response;
  count_response.count = HostToNetwork(static_cast<uint16_t>(2));
  count_response.changed = 0;
  string s(reinterpret_cast<char*>(&count_response), sizeof(count_response));
  m_impl.AddExpectedGet(s,
                        UNIVERSE,
                        m_uid,
                        ola::rdm::ROOT_RDM_DEVICE,
                        ola::rdm::PID_PROXIED_DEVICE_COUNT);
  CPPUNIT_ASSERT(m_api.GetProxiedDeviceCount(
    UNIVERSE,
    m_uid,
    NewSingleCallback(this, &RDMAPITest::CheckProxiedDeviceCount),
    &error));

  // get proxied devices
  CPPUNIT_ASSERT(!m_api.GetProxiedDevices(
    UNIVERSE,
    m_bcast_uid,
    NewSingleCallback(this, &RDMAPITest::CheckProxiedDevices),
    &error));
  CheckForBroadcastError(&error);
  CPPUNIT_ASSERT(!m_api.GetProxiedDevices(
    UNIVERSE,
    m_bcast_uid,
    NewSingleCallback(this, &RDMAPITest::CheckProxiedDevices),
    &error));
  CheckForBroadcastError(&error);

  struct {
    uint8_t uid1[UID::UID_SIZE];
    uint8_t uid2[UID::UID_SIZE];
  } uids_response;
  m_test_uid1.Pack(uids_response.uid1, UID::UID_SIZE);
  m_test_uid2.Pack(uids_response.uid2, UID::UID_SIZE);
  string s2(reinterpret_cast<char*>(&uids_response), sizeof(uids_response));
  m_impl.AddExpectedGet(s2,
                        UNIVERSE,
                        m_uid,
                        ola::rdm::ROOT_RDM_DEVICE,
                        ola::rdm::PID_PROXIED_DEVICES);
  CPPUNIT_ASSERT(m_api.GetProxiedDevices(
    UNIVERSE,
    m_uid,
    NewSingleCallback(this, &RDMAPITest::CheckProxiedDevices),
    &error));
}


/*
 * test that network commands work
 */
void RDMAPITest::testNetworkCommands() {
  string error;
  // get comms status
  CPPUNIT_ASSERT(!m_api.GetCommStatus(
    UNIVERSE,
    m_group_uid,
    NewSingleCallback(this, &RDMAPITest::CheckCommsStatus),
    &error));
  CheckForBroadcastError(&error);
  CPPUNIT_ASSERT(!m_api.GetCommStatus(
    UNIVERSE,
    m_group_uid,
    NewSingleCallback(this, &RDMAPITest::CheckCommsStatus),
    &error));
  CheckForBroadcastError(&error);

  struct {
    uint16_t short_message;
    uint16_t length_mismatch;
    uint16_t checksum_fail;
  } comms_response;
  comms_response.short_message = HostToNetwork(static_cast<uint16_t>(14));
  comms_response.length_mismatch = HostToNetwork(static_cast<uint16_t>(187));
  comms_response.checksum_fail = HostToNetwork(static_cast<uint16_t>(92));
  string s(reinterpret_cast<char*>(&comms_response), sizeof(comms_response));
  m_impl.AddExpectedGet(s,
                        UNIVERSE,
                        m_uid,
                        ola::rdm::ROOT_RDM_DEVICE,
                        ola::rdm::PID_COMMS_STATUS);
  CPPUNIT_ASSERT(m_api.GetCommStatus(
    UNIVERSE,
    m_uid,
    NewSingleCallback(this, &RDMAPITest::CheckCommsStatus),
    &error));

  // clear comms status
  m_impl.AddExpectedSet(UNIVERSE,
                        m_bcast_uid,
                        ola::rdm::ROOT_RDM_DEVICE,
                        ola::rdm::PID_COMMS_STATUS);
  CPPUNIT_ASSERT(m_api.ClearCommStatus(
    UNIVERSE,
    m_bcast_uid,
    NewSingleCallback(this, &RDMAPITest::CheckWasBroadcast),
    &error));

  m_impl.AddExpectedSet(UNIVERSE,
                        m_uid,
                        ola::rdm::ROOT_RDM_DEVICE,
                        ola::rdm::PID_COMMS_STATUS);
  CPPUNIT_ASSERT(m_api.ClearCommStatus(
    UNIVERSE,
    m_uid,
    NewSingleCallback(this, &RDMAPITest::CheckResponseStatus),
    &error));

  // TODO(simon): test status message here

  // status id description
  uint16_t status_id = 12;
  CPPUNIT_ASSERT(!m_api.GetStatusIdDescription(
    UNIVERSE,
    m_bcast_uid,
    status_id,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));
  CheckForBroadcastError(&error);
  CPPUNIT_ASSERT(!m_api.GetStatusIdDescription(
    UNIVERSE,
    m_bcast_uid,
    status_id,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));
  CheckForBroadcastError(&error);

  m_impl.AddExpectedGet(string(TEST_DESCRIPTION),
                        UNIVERSE,
                        m_uid,
                        ola::rdm::ROOT_RDM_DEVICE,
                        ola::rdm::PID_STATUS_ID_DESCRIPTION);
  CPPUNIT_ASSERT(m_api.GetStatusIdDescription(
    UNIVERSE,
    m_uid,
    status_id,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));

  // clear status id
  uint16_t sub_device = 3;
  m_impl.AddExpectedSet(UNIVERSE,
                        m_bcast_uid,
                        sub_device,
                        ola::rdm::PID_CLEAR_STATUS_ID);
  CPPUNIT_ASSERT(m_api.ClearStatusId(
    UNIVERSE,
    m_bcast_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckWasBroadcast),
    &error));
  m_impl.AddExpectedSet(UNIVERSE,
                        m_uid,
                        ola::rdm::ROOT_RDM_DEVICE,
                        ola::rdm::PID_CLEAR_STATUS_ID);
  CPPUNIT_ASSERT(m_api.ClearStatusId(
    UNIVERSE,
    m_uid,
    ola::rdm::ROOT_RDM_DEVICE,
    NewSingleCallback(this, &RDMAPITest::CheckResponseStatus),
    &error));

  // TODO(simon): add sub device reporting threshold here
}


/*
 * Test RDM Information commands work correctly
 */
void RDMAPITest::testRDMInformation() {
  string error;
  // supported params
  struct {
    uint16_t param1;
    uint16_t param2;
    uint16_t param3;
  } pid_list;
  pid_list.param1 = HostToNetwork(static_cast<uint16_t>(0x1234));
  pid_list.param2 = HostToNetwork(static_cast<uint16_t>(0xabcd));
  pid_list.param3 = HostToNetwork(static_cast<uint16_t>(0x00aa));
  uint16_t sub_device = 1;
  CPPUNIT_ASSERT(!m_api.GetSupportedParameters(
    UNIVERSE,
    m_bcast_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckSupportedParams),
    &error));
  CheckForBroadcastError(&error);
  CPPUNIT_ASSERT(!m_api.GetSupportedParameters(
    UNIVERSE,
    m_bcast_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckSupportedParams),
    &error));
  CheckForBroadcastError(&error);
  string s(reinterpret_cast<char*>(&pid_list), sizeof(pid_list));
  m_impl.AddExpectedGet(s,
                        UNIVERSE,
                        m_uid,
                        ola::rdm::ROOT_RDM_DEVICE,
                        ola::rdm::PID_SUPPORTED_PARAMETERS);
  CPPUNIT_ASSERT(m_api.GetSupportedParameters(
    UNIVERSE,
    m_uid,
    ola::rdm::ROOT_RDM_DEVICE,
    NewSingleCallback(this, &RDMAPITest::CheckSupportedParams),
    &error));

  // parameter description
  uint16_t pid = 16;
  CPPUNIT_ASSERT(!m_api.GetParameterDescription(
    UNIVERSE,
    m_bcast_uid,
    pid,
    NewSingleCallback(this, &RDMAPITest::CheckMalformedParameterDescription),
    &error));
  CheckForBroadcastError(&error);
  CPPUNIT_ASSERT(!m_api.GetParameterDescription(
    UNIVERSE,
    m_bcast_uid,
    pid,
    NewSingleCallback(this, &RDMAPITest::CheckMalformedParameterDescription),
    &error));
  CheckForBroadcastError(&error);

  s = "";
  m_impl.AddExpectedGet(s,
                        UNIVERSE,
                        m_uid,
                        ola::rdm::ROOT_RDM_DEVICE,
                        ola::rdm::PID_PARAMETER_DESCRIPTION);
  CPPUNIT_ASSERT(m_api.GetParameterDescription(
    UNIVERSE,
    m_uid,
    pid,
    NewSingleCallback(this, &RDMAPITest::CheckMalformedParameterDescription),
    &error));

  struct param_info_s {
    uint16_t pid;
    uint8_t pdl_size;
    uint8_t data_type;
    uint8_t command_class;
    uint8_t type;
    uint8_t unit;
    uint8_t prefix;
    uint32_t min_value;
    uint32_t max_value;
    uint32_t default_value;
    char label[32];
  } __attribute__((packed));
  struct param_info_s param_info;
  param_info.pid = HostToNetwork(static_cast<uint16_t>(0x1234));
  param_info.pdl_size = 10;
  param_info.data_type = ola::rdm::DS_UNSIGNED_DWORD;
  param_info.command_class = ola::rdm::CC_GET;
  param_info.type = 0;
  param_info.unit = ola::rdm::UNITS_METERS;
  param_info.prefix = ola::rdm::PREFIX_KILO;
  param_info.min_value = HostToNetwork(static_cast<uint32_t>(0));
  param_info.max_value = HostToNetwork(static_cast<uint32_t>(200000));
  param_info.default_value = HostToNetwork(static_cast<uint32_t>(1000));
  strncpy(param_info.label, TEST_DESCRIPTION, sizeof(param_info.label));

  string s2(reinterpret_cast<char*>(&param_info),
            sizeof(param_info) - sizeof(param_info.label) +
            strlen(TEST_DESCRIPTION));
  m_impl.AddExpectedGet(s2,
                        UNIVERSE,
                        m_uid,
                        ola::rdm::ROOT_RDM_DEVICE,
                        ola::rdm::PID_PARAMETER_DESCRIPTION);
  CPPUNIT_ASSERT(m_api.GetParameterDescription(
    UNIVERSE,
    m_uid,
    ola::rdm::ROOT_RDM_DEVICE,
    NewSingleCallback(this, &RDMAPITest::CheckParameterDescription),
    &error));
}


/*
 * Check that the product information commands work correctly
 */
void RDMAPITest::testProductInformation() {
  string error;
  uint16_t sub_device = 1;

  // device info
  struct device_info_s {
    uint8_t version_high;
    uint8_t version_low;
    uint16_t model;
    uint16_t product_category;
    uint32_t software_version;
    uint16_t dmx_footprint;
    uint8_t current_personality;
    uint8_t personaility_count;
    uint16_t dmx_start_address;
    uint16_t sub_device_count;
    uint8_t sensor_count;
  } __attribute__((packed));
  struct device_info_s device_info;
  device_info.version_high = 1;
  device_info.version_low = 0;
  device_info.model = HostToNetwork(static_cast<uint16_t>(2));
  device_info.product_category = HostToNetwork(static_cast<uint16_t>(3));
  device_info.software_version = HostToNetwork(
    static_cast<uint32_t>(0x12345678));
  device_info.dmx_footprint = HostToNetwork(static_cast<uint16_t>(400));
  device_info.current_personality = 1;
  device_info.personaility_count = 2;
  device_info.dmx_start_address = HostToNetwork(static_cast<uint16_t>(12));
  device_info.sub_device_count = HostToNetwork(static_cast<uint16_t>(10));
  device_info.sensor_count = 4;

  CPPUNIT_ASSERT(!m_api.GetDeviceInfo(
    UNIVERSE,
    m_bcast_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckDeviceInfo),
    &error));
  CheckForBroadcastError(&error);
  CPPUNIT_ASSERT(!m_api.GetDeviceInfo(
    UNIVERSE,
    m_group_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckDeviceInfo),
    &error));
  CheckForBroadcastError(&error);
  string s(reinterpret_cast<char*>(&device_info), sizeof(device_info));
  m_impl.AddExpectedGet(s,
                        UNIVERSE,
                        m_uid,
                        ola::rdm::ROOT_RDM_DEVICE,
                        ola::rdm::PID_DEVICE_INFO);
  CPPUNIT_ASSERT(m_api.GetDeviceInfo(
    UNIVERSE,
    m_uid,
    ola::rdm::ROOT_RDM_DEVICE,
    NewSingleCallback(this, &RDMAPITest::CheckDeviceInfo),
    &error));

  // product detail id list
  struct {
    uint16_t detail1;
    uint16_t detail2;
    uint16_t detail3;
  } detail_list;
  detail_list.detail1 = HostToNetwork(static_cast<uint16_t>(0x5678));
  detail_list.detail2 = HostToNetwork(static_cast<uint16_t>(0xfedc));
  detail_list.detail3 = HostToNetwork(static_cast<uint16_t>(0xaa00));
  CPPUNIT_ASSERT(!m_api.GetProductDetailIdList(
    UNIVERSE,
    m_bcast_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckProductDetailList),
    &error));
  CheckForBroadcastError(&error);
  CPPUNIT_ASSERT(!m_api.GetProductDetailIdList(
    UNIVERSE,
    m_group_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckProductDetailList),
    &error));
  CheckForBroadcastError(&error);
  string s2(reinterpret_cast<char*>(&detail_list), sizeof(detail_list));
  m_impl.AddExpectedGet(s2,
                        UNIVERSE,
                        m_uid,
                        ola::rdm::ROOT_RDM_DEVICE,
                        ola::rdm::PID_PRODUCT_DETAIL_ID_LIST);
  CPPUNIT_ASSERT(m_api.GetProductDetailIdList(
    UNIVERSE,
    m_uid,
    ola::rdm::ROOT_RDM_DEVICE,
    NewSingleCallback(this, &RDMAPITest::CheckProductDetailList),
    &error));

  // device model description
  CPPUNIT_ASSERT(!m_api.GetDeviceModelDescription(
    UNIVERSE,
    m_bcast_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));
  CheckForBroadcastError(&error);
  CPPUNIT_ASSERT(!m_api.GetDeviceModelDescription(
    UNIVERSE,
    m_group_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));
  CheckForBroadcastError(&error);

  m_impl.AddExpectedGet(string(TEST_DESCRIPTION),
                        UNIVERSE,
                        m_uid,
                        sub_device,
                        ola::rdm::PID_DEVICE_MODEL_DESCRIPTION);
  CPPUNIT_ASSERT(m_api.GetDeviceModelDescription(
    UNIVERSE,
    m_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));

  // manufacturer label
  CPPUNIT_ASSERT(!m_api.GetManufacturerLabel(
    UNIVERSE,
    m_bcast_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));
  CheckForBroadcastError(&error);
  CPPUNIT_ASSERT(!m_api.GetManufacturerLabel(
    UNIVERSE,
    m_group_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));
  CheckForBroadcastError(&error);

  m_impl.AddExpectedGet(string(TEST_DESCRIPTION),
                        UNIVERSE,
                        m_uid,
                        sub_device,
                        ola::rdm::PID_MANUFACTURER_LABEL);
  CPPUNIT_ASSERT(m_api.GetManufacturerLabel(
    UNIVERSE,
    m_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));

  // get device label
  CPPUNIT_ASSERT(!m_api.GetDeviceLabel(
    UNIVERSE,
    m_bcast_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));
  CheckForBroadcastError(&error);
  CPPUNIT_ASSERT(!m_api.GetDeviceLabel(
    UNIVERSE,
    m_group_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));
  CheckForBroadcastError(&error);

  m_impl.AddExpectedGet(string(TEST_DESCRIPTION),
                        UNIVERSE,
                        m_uid,
                        sub_device,
                        ola::rdm::PID_DEVICE_LABEL);
  CPPUNIT_ASSERT(m_api.GetDeviceLabel(
    UNIVERSE,
    m_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));

  // set device label
  s = TEST_DESCRIPTION;
  m_impl.AddExpectedSet(UNIVERSE,
                        m_uid,
                        sub_device,
                        ola::rdm::PID_DEVICE_LABEL,
                        reinterpret_cast<const uint8_t*>(s.data()),
                        s.size());
  CPPUNIT_ASSERT(m_api.SetDeviceLabel(
    UNIVERSE,
    m_uid,
    sub_device,
    s,
    NewSingleCallback(this, &RDMAPITest::CheckResponseStatus),
    &error));
  // check we can bcast
  m_impl.AddExpectedSet(UNIVERSE,
                        m_bcast_uid,
                        ola::rdm::ALL_RDM_SUBDEVICES,
                        ola::rdm::PID_DEVICE_LABEL,
                        reinterpret_cast<const uint8_t*>(s.data()),
                        s.size());
  CPPUNIT_ASSERT(m_api.SetDeviceLabel(
    UNIVERSE,
    m_bcast_uid,
    ola::rdm::ALL_RDM_SUBDEVICES,
    s,
    NewSingleCallback(this, &RDMAPITest::CheckWasBroadcast),
    &error));
  m_impl.AddExpectedSet(UNIVERSE,
                        m_group_uid,
                        ola::rdm::ALL_RDM_SUBDEVICES,
                        ola::rdm::PID_DEVICE_LABEL,
                        reinterpret_cast<const uint8_t*>(s.data()),
                        s.size());
  CPPUNIT_ASSERT(m_api.SetDeviceLabel(
    UNIVERSE,
    m_group_uid,
    ola::rdm::ALL_RDM_SUBDEVICES,
    s,
    NewSingleCallback(this, &RDMAPITest::CheckWasBroadcast),
    &error));
  // check out of range sub devices fail
  CPPUNIT_ASSERT(!m_api.SetDeviceLabel(
    UNIVERSE,
    m_group_uid,
    0x0201,
    s,
    NewSingleCallback(this, &RDMAPITest::CheckResponseStatus),
    &error));
  CheckForDeviceRangeBcastError(&error);

  // software version label
  CPPUNIT_ASSERT(!m_api.GetSoftwareVersionLabel(
    UNIVERSE,
    m_bcast_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));
  CheckForBroadcastError(&error);
  CPPUNIT_ASSERT(!m_api.GetSoftwareVersionLabel(
    UNIVERSE,
    m_group_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));
  CheckForBroadcastError(&error);

  m_impl.AddExpectedGet(string(TEST_DESCRIPTION),
                        UNIVERSE,
                        m_uid,
                        sub_device,
                        ola::rdm::PID_SOFTWARE_VERSION_LABEL);
  CPPUNIT_ASSERT(m_api.GetSoftwareVersionLabel(
    UNIVERSE,
    m_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));

  // Boot software label
  CPPUNIT_ASSERT(!m_api.GetBootSoftwareVersionLabel(
    UNIVERSE,
    m_bcast_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));
  CheckForBroadcastError(&error);
  CPPUNIT_ASSERT(!m_api.GetBootSoftwareVersionLabel(
    UNIVERSE,
    m_group_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));
  CheckForBroadcastError(&error);

  m_impl.AddExpectedGet(string(TEST_DESCRIPTION),
                        UNIVERSE,
                        m_uid,
                        sub_device,
                        ola::rdm::PID_BOOT_SOFTWARE_VERSION_LABEL);
  CPPUNIT_ASSERT(m_api.GetBootSoftwareVersionLabel(
    UNIVERSE,
    m_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckLabel),
    &error));
}

/*
 * Check that DMX commands work
 */
void RDMAPITest::testDmxSetup() {
  string error;
  uint16_t sub_device = 1;

  // Check get start address
  CPPUNIT_ASSERT(!m_api.GetDMXAddress(
    UNIVERSE,
    m_bcast_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckDMXStartAddress),
    &error));
  CheckForBroadcastError(&error);
  CPPUNIT_ASSERT(!m_api.GetDMXAddress(
    UNIVERSE,
    m_bcast_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckDMXStartAddress),
    &error));
  CheckForBroadcastError(&error);

  uint16_t start_address = HostToNetwork(static_cast<uint16_t>(44));
  string s(reinterpret_cast<char*>(&start_address), sizeof(start_address));
  m_impl.AddExpectedGet(s,
                        UNIVERSE,
                        m_uid,
                        sub_device,
                        ola::rdm::PID_DMX_START_ADDRESS);
  CPPUNIT_ASSERT(m_api.GetDMXAddress(
    UNIVERSE,
    m_uid,
    sub_device,
    NewSingleCallback(this, &RDMAPITest::CheckDMXStartAddress),
    &error));

  // Check set start address
  start_address = 64;
  uint16_t address_data = HostToNetwork(start_address);
  m_impl.AddExpectedSet(UNIVERSE,
                        m_uid,
                        sub_device,
                        ola::rdm::PID_DMX_START_ADDRESS,
                        reinterpret_cast<const uint8_t*>(&address_data),
                        sizeof(address_data));
  CPPUNIT_ASSERT(m_api.SetDMXAddress(
    UNIVERSE,
    m_uid,
    sub_device,
    start_address,
    NewSingleCallback(this, &RDMAPITest::CheckResponseStatus),
    &error));
  // check bcasts work
  m_impl.AddExpectedSet(UNIVERSE,
                        m_bcast_uid,
                        ola::rdm::ALL_RDM_SUBDEVICES,
                        ola::rdm::PID_DMX_START_ADDRESS,
                        reinterpret_cast<const uint8_t*>(&address_data),
                        sizeof(address_data));
  CPPUNIT_ASSERT(m_api.SetDMXAddress(
    UNIVERSE,
    m_bcast_uid,
    ola::rdm::ALL_RDM_SUBDEVICES,
    start_address,
    NewSingleCallback(this, &RDMAPITest::CheckWasBroadcast),
    &error));
  m_impl.AddExpectedSet(UNIVERSE,
                        m_group_uid,
                        0x0200,
                        ola::rdm::PID_DMX_START_ADDRESS,
                        reinterpret_cast<const uint8_t*>(&address_data),
                        sizeof(address_data));
  CPPUNIT_ASSERT(m_api.SetDMXAddress(
    UNIVERSE,
    m_group_uid,
    0x0200,
    start_address,
    NewSingleCallback(this, &RDMAPITest::CheckWasBroadcast),
    &error));
  CPPUNIT_ASSERT(!m_api.SetDMXAddress(
    UNIVERSE,
    m_group_uid,
    0x0201,
    start_address,
    NewSingleCallback(this, &RDMAPITest::CheckWasBroadcast),
    &error));
  CheckForDeviceRangeBcastError(&error);
}
