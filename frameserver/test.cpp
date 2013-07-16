#include <glibmm.h>
#include <vector>
#include <iostream>

#include <time.h>
#include <stdlib.h>

#include "defines.h"
#include "Buffer.h"
#include "Buffers.h"

#define NUMTHREADS 20
#define USECS 1000

using namespace std;

// we use a pointer because we can only use objects from the threading library
// once the threading has been initialised, this is only done in main.
Buffers* buffers;

void reader(void);
void writer(void);

int main(void)
{
  srand( time(NULL) );
  Glib::init();

  // our main loop with support for signals and all that jazz
  //Glib::RefPtr<Glib::MainLoop> Main = Glib::MainLoop::create();

  buffers = new Buffers(NUMBUFS);

  vector<Glib::Threads::Thread*> readers;
  vector<Glib::Threads::Thread*> writers;

  for(int i = 0; i < NUMTHREADS; i++)
  {
    readers.push_back( Glib::Threads::Thread::create( sigc::ptr_fun( &reader), false ) );
    writers.push_back( Glib::Threads::Thread::create( sigc::ptr_fun( &writer), false ) );
  }

  //Main->run();

  int count = 0;
  while(1) {
    count++;
    if( count % 100 == 0 )
    {
      frame_t frame = buffers->get(rand()%NUMBUFS)->get();
      for(int i = 0; i < HEIGHT; i++)
      {
        for(int j = 0; j < WIDTH; j++)
          cout << ((frame.windows[i][j] >= 120) ? 'X' : '.');
        cout << endl;
      }
      cout << endl;

      for(int i = 0; i < SEGWIDTH; i++)
        cout << frame.segments[i].r;
      cout << endl << endl;
    }
    Glib::usleep( 10000 );
  }//*/

  return 0;
}

void reader(void)
{
  bool quit = false;
  frame_t frame;
  int bufnum = 0;
  while( !quit )
  {
    bufnum = rand()%NUMBUFS;
    frame = buffers->get(bufnum)->get();
//    cout << "read " << bufnum << endl;
    Glib::usleep( rand()%USECS );
  }
}

void writer(void)
{
  frame_t frame;
  bool quit = false;
  int bufnum = 0;
  while( !quit )
  {
    bufnum = rand()%NUMBUFS;
    for(int i = 0; i < 7; i++)
    {
      for(int j = 0; j < 12; j++)
      {
        frame.windows[i][j] = rand()%255;
      }
    }

    for(int i = 0; i < 12; i++)
    {
      frame.segments[i].r = 33+rand()%90;
      frame.segments[i].g = 33+rand()%90;
      frame.segments[i].b = 33+rand()%90;
    }

    buffers->get(bufnum)->set( frame );
//    cout << "wrote " << bufnum << endl;
    Glib::usleep( rand()%USECS );
  }
}
