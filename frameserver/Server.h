#ifndef __SERVER_H_
#define __SERVER_H_

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "Buffers.h"
#include "Buffer.h"
#include "defines.h"


using boost::asio::ip::udp;
using namespace std;

class Server : public sigc::trackable
{
public:
  Server(int _port);
  ~Server();
  
  void listen();
  void mix();
  void launch_threads();
  void control();
  
private:
  Glib::Mutex mutex_;
  
  vector<Glib::Thread*> threads;
  vector<Buffer*> buffers;
  vector<udp::endpoint> endpoints;
  
  int numbufs;
  
  int port;
};

#endif
