#ifndef __BUFFER_H_
#define __BUFFER_H_
#include <string>
#include <glibmm.h>
#include "defines.h"

/*  This is a threadsafe wrapper around the "frame_t" struct
	which automates locking during get and set. */

class Buffer : public sigc::trackable
{
  public:
    Buffer(std::string _id);
    ~Buffer();
    
    void set(frame_t);
    frame_t get();
    
    std::string get_id();
    void set_id(std::string id);
    
  private:
    std::string id;
    frame_t frame;
  
    Glib::Mutex mutex_;
};
#endif
