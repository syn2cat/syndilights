#ifndef __BUFFER_H_
#define __BUFFER_H_
#include <string>
#include <glibmm.h>
#include "defines.h"

/*  This is a threadsafe wrapper around the "frame_t" struct
	which automates locking during get and set. */

/* TODO: add forced z value to buffer object so that we can override the
 * senders buffer
*/

class Buffer : public sigc::trackable
{
  public:
    Buffer(std::string _id);
    ~Buffer();

    void set(frame_t);
    frame_t get();

    std::string get_id();
    void set_id(std::string id);

    bool get_selected();
    void set_selected(bool);

  private:
    std::string id;
    frame_t frame;

    bool selected;

    Glib::Threads::Mutex mutex_;
};
#endif
