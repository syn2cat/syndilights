#include <iostream>
#include <stdlib.h>
#include <ola/DmxBuffer.h>
#include <ola/Logging.h>
#include <ola/StreamingClient.h>
#include <math.h>

#include <iostream>                                                                                                          

using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
unsigned int universe = 0;  // universe to use for sending data
unsigned int i;

// turn on OLA logging
ola::InitLogging(ola::OLA_LOG_WARN, ola::OLA_LOG_STDERR);

// Create a new DmxBuffer to hold the data
ola::DmxBuffer buffer;
// set all channels to 0
buffer.Blackout();

// create a new client and set the Error Closure
ola::StreamingClient ola_client;

// Setup the client, this connects to the server
if (!ola_client.Setup()) {
cout << "Setup failed" << endl;
exit(1);
}



// send the data to the ola server 

if (!ola_client.SendDmx(0, buffer)) {
  cout << "Send DMX failed" << endl;
  exit(1);
  }

// close the connection
ola_client.Stop();
return 0;
}

