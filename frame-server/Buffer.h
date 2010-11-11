#ifndef __BUFFER_H_
#define __BUFFER_H_
#include <glibmm.h>
#include "defines.h"

class Buffer : public sigc::trackable
{
  public:
    Buffer(int _id);
    ~Buffer();
    
    void set(frame_t);
    frame_t get();
    int get_id();
    
  private:
    int id;
    frame_t frame;
  
    Glib::Mutex mutex_;
};
#endif