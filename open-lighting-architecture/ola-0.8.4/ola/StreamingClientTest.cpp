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
 * StreamingClientTest.cpp
 * Test fixture for the StreamingClient class
 * Copyright (C) 2005-2010 Simon Newton
 */

#include <cppunit/extensions/HelperMacros.h>
#include <string>

#include "ola/StreamingClient.h"
#include "ola/DmxBuffer.h"
#include "ola/OlaThread.h"
#include "ola/Logging.h"
#include "olad/OlaDaemon.h"


static unsigned int TEST_UNIVERSE = 1;

using ola::OlaDaemon;


class StreamingClientTest: public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(StreamingClientTest);
  CPPUNIT_TEST(testSendDMX);
  CPPUNIT_TEST_SUITE_END();

  public:
    void setUp();
    void tearDown();
    void testSendDMX();

  private:
    class OlaServerThread *m_server_thread;
};


CPPUNIT_TEST_SUITE_REGISTRATION(StreamingClientTest);


/*
 * The thread that the OlaServer runs in.
 */
class OlaServerThread: public ola::OlaThread {
  public:
    OlaServerThread() :
        OlaThread(),
        m_olad(NULL) {
    }
    ~OlaServerThread();
    bool Setup();
    void *Run();
    void Terminate();

  private:
    OlaDaemon *m_olad;
};


OlaServerThread::~OlaServerThread() {
  if (m_olad) {
    delete m_olad;
    m_olad = NULL;
  }
}


bool OlaServerThread::Setup() {
  ola::ola_server_options ola_options;
  ola_options.http_enable = false;
  ola_options.http_localhost_only = false;
  ola_options.http_enable_quit = false;
  ola_options.http_port = 0;
  ola_options.http_data_dir = "";

  m_olad = new OlaDaemon(ola_options);
  if (!m_olad->Init()) {
    delete m_olad;
    m_olad = NULL;
    return false;
  }
  return true;
}


/*
 * Run the ola Server
 */
void *OlaServerThread::Run() {
  if (m_olad) {
    m_olad->Run();
    m_olad->Shutdown();
  }
  return NULL;
}


/*
 * Stop the OLA server
 */
void OlaServerThread::Terminate() {
  if (m_olad)
    m_olad->Terminate();
}



/*
 * Startup the Ola server
 */
void StreamingClientTest::setUp() {
  ola::InitLogging(ola::OLA_LOG_WARN, ola::OLA_LOG_STDERR);
  m_server_thread = new OlaServerThread();
  if (m_server_thread->Setup())
    m_server_thread->Start();
}


/*
 * Stop the ola Server
 */
void StreamingClientTest::tearDown() {
  m_server_thread->Terminate();
  m_server_thread->Join();
  delete m_server_thread;
}


/*
 * Check that the SendDMX method works correctly.
 */
void StreamingClientTest::testSendDMX() {
  ola::StreamingClient ola_client;

  ola::DmxBuffer buffer;
  buffer.Blackout();

  // Setup the client, this connects to the server
  CPPUNIT_ASSERT(ola_client.Setup());
  // Try it again to make sure it doesn't break
  CPPUNIT_ASSERT(!ola_client.Setup());

  CPPUNIT_ASSERT(ola_client.SendDmx(TEST_UNIVERSE, buffer));
  ola_client.Stop();

  // Now reconnect
  CPPUNIT_ASSERT(ola_client.Setup());
  CPPUNIT_ASSERT(ola_client.SendDmx(TEST_UNIVERSE, buffer));
  ola_client.Stop();

  // Now Terminate the server mid flight
  CPPUNIT_ASSERT(ola_client.Setup());
  CPPUNIT_ASSERT(ola_client.SendDmx(TEST_UNIVERSE, buffer));
  m_server_thread->Terminate();
  m_server_thread->Join();

  CPPUNIT_ASSERT(!ola_client.SendDmx(TEST_UNIVERSE, buffer));
  ola_client.Stop();

  CPPUNIT_ASSERT(!ola_client.Setup());
}
