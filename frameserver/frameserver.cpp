#include <glibmm.h>
#include <vector>
#include <iostream>

#include <time.h>
#include <stdlib.h>

#include "defines.h"

#include "Server.h"

#include "Buffer.h"
#include "Buffers.h"

using namespace std;

int main(void) 
{
  srand( time(NULL) );
  Glib::thread_init();

  // our main loop with support for signals and all that jazz
  Glib::RefPtr<Glib::MainLoop> Main = Glib::MainLoop::create();
    
  Server server(4321);  
  Main->run();  
  
  return 0;
}
