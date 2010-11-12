#include "Buffer.h"

Buffer::Buffer( int _id )
{
  id = _id;
}

Buffer::~Buffer() 
{
}

void Buffer::set(frame_t data)
{
  {
    Glib::Mutex::Lock lock(mutex_);
    frame = data;
  }
}

frame_t Buffer::get()
{
  Glib::Mutex::Lock lock(mutex_);
  return frame;
}

int Buffer::get_id()
{
  Glib::Mutex::Lock lock(mutex_);
  return id;
}
