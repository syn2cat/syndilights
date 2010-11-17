#ifndef __SERVER_H_
#define __SERVER_H_

#include <iostream>
#include <string>
#include <vector>
#include <list>

#include <glibmm/timeval.h>

#include <boost/array.hpp>
#include <boost/asio.hpp>

#include <ncurses.h>

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

  void launch_threads();
  
private:
  void console();
  void input();
  void test();
  
  char brtoc(unsigned char);
  int brtoi(unsigned char);
  
  void console_printframe(frame_t _frame);
  void console_printframe_values(frame_t _frame);
  void console_printstats();
  
  void listen();
  void mix();
  int get_size();
  void expire();


  Glib::Mutex mutex_;

  bool consoleinit;
  int console_input;
  long displaycounter, packetcounter;
  
  vector<Glib::Thread*> threads;
  
  vector<Buffer*> buffers;
  vector<udp::endpoint> endpoints;
  vector<time_t> times;
  
  time_t currenttime;

  frame_t frame;
  
  int port;
  char mode;
};

#endif
