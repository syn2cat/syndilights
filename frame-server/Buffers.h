#include <glibmm.h>
#include <vector>

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
    void remove(int);
    
    Buffer* get(int);
    
  private:
    vector<Buffer*> buffers;
    int id;
    Glib::Mutex mutex_;
};
