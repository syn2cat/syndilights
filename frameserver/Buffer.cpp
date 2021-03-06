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
    Glib::Threads::Mutex::Lock lock(mutex_);
    frame = data;
  }
}

void Buffer::set_id(std::string _id)
{
	Glib::Threads::Mutex::Lock lock(mutex_);
	id = _id;
}

void Buffer::set_selected(bool _selected)
{
  Glib::Threads::Mutex::Lock lock(mutex_);
  selected = _selected;
}


frame_t Buffer::get()
{
  Glib::Threads::Mutex::Lock lock(mutex_);
  return frame;
}

std::string Buffer::get_id()
{
  Glib::Threads::Mutex::Lock lock(mutex_);
  return id;
}

bool Buffer::get_selected()
{
  Glib::Threads::Mutex::Lock lock(mutex_);
  return selected;
}
