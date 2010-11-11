#include <glibmm.h>
#include <vector>
#include <iostream>

#include "defines.h"
#include "Buffer.h"
#include "Buffers.h"

#define NUMBUFS 10000
#define NUMTHREADS 10

using namespace std;



Buffers* buffers;

void reader(void);
void writer(void);

int main(void) 
{
  Glib::thread_init();
  Glib::RefPtr<Glib::MainLoop> Main = Glib::MainLoop::create();
  
  buffers = new Buffers(NUMBUFS);

  vector<Glib::Thread*> readers;
  vector<Glib::Thread*> writers;

  for(int i = 0; i < NUMTHREADS; i++)
  {
    readers.push_back( Glib::Thread::create( sigc::ptr_fun( &reader), false ) );
    writers.push_back( Glib::Thread::create( sigc::ptr_fun( &writer), false ) );
  }
  
  Main->run();
  
  /*while(1) {
    usleep( 10000 );
  }*/
  
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
    usleep( rand()%250 );
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
      frame.segments[i].r = rand()%255;
      frame.segments[i].g = rand()%255;
      frame.segments[i].b = rand()%255;
    }
    
    buffers->get(bufnum)->set( frame );
//    cout << "wrote " << bufnum << endl;
    usleep( rand()%250 );
  }
}