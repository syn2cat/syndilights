#include "Buffers.h"

Buffers::Buffers() 
{
  id = 0;
}

Buffers::Buffers(int _bufnum)
{
  id = 0;
  for( int i = 0; i < _bufnum; i++)
    add();
}

Buffers::~Buffers()
{
  Glib::Mutex::Lock lock(mutex_);
  buffers.clear();
}

Buffer* Buffers::get(int index)
{
  Glib::Mutex::Lock lock(mutex_);
  return buffers[index];
}

void Buffers::add()
{
  Glib::Mutex::Lock lock(mutex_);
  id += 1;
  buffers.push_back( new Buffer(id) );
}

void Buffers::remove(int _id)
{
  Glib::Mutex::Lock lock(mutex_);
  int size = buffers.size();
  for( int i = 0; i < size; i++ )
  {
    if( buffers[i]->get_id() == _id )
      buffers.erase( buffers.begin()+i );
  }
}