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

double t = 0.0;
while(1) {
  t += 0.1;
  buffer.SetChannel(4, (int)(127+128*sin(t)));
  buffer.SetChannel(5, (int)(127+128*cos(t/4)));
//for(int j = 0; j < 255; j++) {
if (!ola_client.SendDmx(0, buffer)) {
  cout << "Send DMX failed" << endl;
  exit(1);
  }
usleep(20000);   // sleep for 20ms between updates                                                                            
//cout << "universe " << j << endl;
//}
}

// close the connection
ola_client.Stop();
return 0;
}

