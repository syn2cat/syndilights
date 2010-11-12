#include "Buffer.h"

Buffer::Buffer( std::string _id )
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

void Buffer::set_id(std::string _id)
{
	Glib::Mutex::Lock lock(mutex_);
	id = _id;
}

frame_t Buffer::get()
{
  Glib::Mutex::Lock lock(mutex_);
  return frame;
}

std::string Buffer::get_id()
{
  Glib::Mutex::Lock lock(mutex_);
  return id;
}
