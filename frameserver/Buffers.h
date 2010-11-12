#ifndef __BUFFERS_H_
#define __BUFFERS_H_

/*  This is a thread-safe wrapper around a standard library
    vector of pointers to "Buffer" objects. It automates locking during
	  all operations and generates "ID" hashes for the buffers upon creation. */

/*	TODO: * create hashes during buffer creation 
          * throw and handle exceptions */

#include <glibmm.h>
#include <vector>
#include <string>

#include <time.h>

#include "defines.h"
#include "Buffer.h"

using namespace std;

class Buffers : public sigc::trackable
{
  public:
    Buffers();
    Buffers(int);
    
    ~Buffers();
    
    void add();
    void remove(std::string);
    
    Buffer* get(int);
    
  private:
    vector<Buffer*> buffers;
    std::string id;
    Glib::Mutex mutex_;
};
#endif
