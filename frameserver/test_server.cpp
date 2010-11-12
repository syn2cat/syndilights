#include <glibmm.h>
#include <vector>
#include <iostream>

#include <time.h>
#include <stdlib.h>

#include "defines.h"

#include "Server.h"

#include "Buffer.h"
#include "Buffers.h"

#define NUMBUFS 10

using namespace std;
Buffers* buffers;

int main(void) 
{
  srand( time(NULL) );
  Glib::thread_init();

  // our main loop with support for signals and all that jazz
  Glib::RefPtr<Glib::MainLoop> Main = Glib::MainLoop::create();
    
  Server server(10,4321);
  server.launch_threads();
  
  Main->run();  
  
  return 0;
}
