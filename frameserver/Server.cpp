#include "Server.h"

Server::Server(int _numbufs, int _port )
{
  for(int i = 0; i < _numbufs; i++)
    buffers.push_back( new Buffer(i) );
    
  port = _port;
}

Server::~Server()
{
  Glib::Mutex::Lock lock(mutex_);
  int size = buffers.size();
  for(int i = 0; i < size; i++)
    delete buffers[i];
}

void Server::launch_threads()
{
  threads.push_back( Glib::Thread::create( sigc::mem_fun(this, &Server::listen), false ) );
  threads.push_back( Glib::Thread::create( sigc::mem_fun(this, &Server::mix), false ) );
}


/* this listens for UDP connections on a port and waits until there is data,
   processes it and repeats */
   
/* TODO 
    error and format checking
    clean exit conditions
    ability for one client to take over the display
    */
   
void Server::listen()
{
  int packetcounter = 0;
  try
  {
    boost::asio::io_service io_service;
     
    // next line is NOT thread-safe because we're accessing "port" without lock
    // however, it is done only once before any other threads are started and
    // should be safe
    udp::socket socket(io_service, udp::endpoint(udp::v4(), port));

    cout << "listening" << endl;
    while (1)
    {
      frame_t frame;
      boost::array<char, BUFLEN> recv_buf;
      udp::endpoint remote_endpoint;
      boost::system::error_code error;
      
      socket.receive_from(boost::asio::buffer(recv_buf),
          remote_endpoint, 0, error);

      packetcounter++;
      if( packetcounter % 1000 == 0 )
        cout << endl << packetcounter << endl;
      
      for(int i = 0; i < HEIGHT; i++)
      {
        for(int j = 0; j < WIDTH; j++)
        {
          frame.windows[i][j] = recv_buf[2+i*(WIDTH+1)+j];
        }
      }
      
      for(int i = 0; i < SEGWIDTH; i++ )
      {
        frame.segments[i].r = recv_buf[2+(WIDTH+1)*HEIGHT+i];
        frame.segments[i].g = recv_buf[2+(WIDTH+1)*HEIGHT+(SEGWIDTH+1)*1+i];
        frame.segments[i].b = recv_buf[2+(WIDTH+1)*HEIGHT+(SEGWIDTH+1)*2+i];
      }
      
      // this part needs to be made threadsafe because buffers will be accessed
      // by the mixer and the udp listener
        {
        Glib::Mutex::Lock lock(mutex_);
        // convert ascii to integer value
        if( recv_buf[0]-49 < buffers.size() ) 
          {
            buffers[ recv_buf[0]-49 ]->set(frame);
          }
        }
        
      
      if (error && error != boost::asio::error::message_size)
        throw boost::system::system_error(error);

      std::string message = "received";

      boost::system::error_code ignored_error;
      // we can provide feedback to clients
      //socket.send_to(boost::asio::buffer(message),
      //    remote_endpoint, 0, ignored_error);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}


/* the framemixer, this periodically (40 times a second) reads all input
   buffers and then produces output, ready to be displayed.
   In the final version, this is where interesting things will happen.
   */
void Server::mix()
{
  int size = 0;
  int counter = 0;
  
  while(1)
  {
    counter++;
    frame_t frame, temp_frame;  
    {
      Glib::Mutex::Lock lock(mutex_);
      size = buffers.size();
    }

    for(int x = 0; x < 6; x++)
    {
      {
        Glib::Mutex::Lock lock(mutex_);
        temp_frame = buffers[x]->get();
      }
      
      for(int i = 0; i < HEIGHT; i++)
      {
        for(int j = 0; j < WIDTH; j++)
        {
           frame.windows[i][j] = (frame.windows[i][j] + temp_frame.windows[i][j])/2;
        }
      }
      
      for(int i = 0; i < SEGWIDTH; i++)
      {
        frame.segments[i].r = (temp_frame.segments[i].r + frame.segments[i].r)/2;
        frame.segments[i].g = (temp_frame.segments[i].g + frame.segments[i].g)/2;
        frame.segments[i].b = (temp_frame.segments[i].b + frame.segments[i].b)/2;
      }
    }

    if( counter % 100 == 0 )
    {
      cout << counter << endl;
      for(int i = 0; i < HEIGHT; i++)
      {
        for(int j = 0; j < WIDTH; j++)
        {
           cout << frame.windows[i][j];
        }
        cout << endl;
      }
      cout << endl;
      
      for(int i = 0; i < SEGWIDTH; i++)
      {
        cout << frame.segments[i].r;
      }
      cout << endl << endl; //*/
    }
    
    usleep( 25000 );
  }
}

