#include "Server.h"

Server::Server(int _port )
{
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
  threads.push_back( Glib::Thread::create( sigc::mem_fun(this, &Server::console), false) );
  threads.push_back( Glib::Thread::create( sigc::mem_fun(this, &Server::expire), false) );
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
  long packetcounter = 0;
  try
  {
    boost::asio::io_service io_service;
     
    // next line is NOT thread-safe because we're accessing "port" without lock
    // however, it is done only once before any other threads are started and
    // should be safe
    udp::socket socket(io_service, udp::endpoint(udp::v4(), port));

    cout << "listening" << endl;
    frame_t frame;
    while (1)
    {
      // creating the buffer each time is faster than zeroing it out
      boost::array<char, BUFLEN> recv_buf;
      udp::endpoint remote_endpoint;
      boost::system::error_code error;
      
      socket.receive_from(boost::asio::buffer(recv_buf),
          remote_endpoint, 0, error);

      // bufnum is used further down
        /* the buffer is locked for a long long time, however, we need
           to make sure that none of the buffers expires while we're about
           to write to it */
      {
        Glib::Mutex::Lock lock(mutex_);
        int size = buffers.size();
        time(&currenttime);
        int bufnum = 0;
        bool known = false;
        for(bufnum = 0; bufnum < size; bufnum++)
        {
          // have we encountered this source before?
          if(endpoints[bufnum] == remote_endpoint)
          {
            known = true;
            break;
          }
        }

        if( !known && size < NUMBUFS )
        {
          // create a new buffer make a note of the endpoint
          std::stringstream endpointstring;
          endpointstring << remote_endpoint;
          cout << "adding new buffer for " << remote_endpoint <<  endl;
          buffers.push_back( new Buffer( endpointstring.str() ) );
          endpoints.push_back( remote_endpoint );
          times.push_back( currenttime );
        }
        
        // discard packet, we're not accepting any more sources!
        else if( !known && size >= NUMBUFS )
        {
          cout << "no more buffers left! " << bufnum << endl;
          continue;
        }
        
        if( packetcounter % 10000 == 0 )
          {
          cout << endl << "packets received " << packetcounter << endl;
          /*cout << remote_endpoint << endl;
          for(int i = 0; i < BUFLEN; i++)
            cout << recv_buf[i]; 
          cout << endl;//*/
          }
        packetcounter++;        
        
        frame.z = recv_buf[0];
        for(int i = 0; i < HEIGHT; i++)
        {
          for(int j = 0; j < WIDTH; j++)
          {
            for(int a = 0; a < CHANNELS; a++)
            {
              frame.windows[i][j][a] = recv_buf[HEADEROFFSET+ i*(CHANNELS*WIDTH+1) + j*CHANNELS + a];
            }
          }
        }
        
        for(int w = 0; w < SEGWIDTH; w++ )
        {
          for(int n = 0;n < SEGNUM; n++)
          {
            for(int a = 0; a < SEGCHANNELS; a++)
            {
              frame.segments[w][n][a] = recv_buf[HEADEROFFSET+WINDOWOFFSET+ w*(SEGCHANNELS*SEGNUM+1) + n*SEGCHANNELS + a];
            }
          }
        }
        
        // be extra certain that we're not writing into wild memory
        if( bufnum < buffers.size() ) 
        {
          buffers[ bufnum ]->set(frame);
          
          // this is accurate enough for the purpose of expiring unused buffers
          times[bufnum] = currenttime;
        }
      } // lock is released  
      
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

    // we lock the buffers for a long time, but we need to make sure
    // that none of the buffers is allowed to expire while we're working on it!
    {
      Glib::Mutex::Lock lock(mutex_);
      size = buffers.size();
      for(int x = 0; x < size; x++)
      {
          temp_frame = buffers[x]->get();
        
        for(int i = 0; i < HEIGHT; i++)
        {
          for(int j = 0; j < WIDTH; j++)
          {
            for(int a = 0; a < CHANNELS; a++)
            {
              // do something interesting here
              frame.windows[i][j][a] = temp_frame.windows[i][j][a];
            }
          }
        }
        
        for(int w = 0; w < SEGWIDTH; w++ )
        {
          for(int n = 0;n < SEGNUM; n++)
          {
            for(int a = 0; a < SEGCHANNELS; a++)
            {
              frame.segments[w][n][a] = temp_frame.segments[w][n][a];
            }
          }
        }

        /*if( counter % 100 == 0 )
        {
          cout << counter << endl;
          for(int i = 0; i < HEIGHT; i++)
          {
            for(int j = 0; j < WIDTH; j++)
            {
               cout << frame.windows[i][j][0];
            }
            cout << endl;
          }
          cout << endl;
          
          for(int w = 0; w < SEGWIDTH; w++)
          {
            for(int n = 0; n < SEGNUM; n++)
            {
              cout << frame.segments[w][n][0];
            }
            cout << endl;
          }
          cout << endl << endl;
        } //*/
      }
		}
  usleep( 25000 );
	}
}

void Server::console()
{
  while(1)
  {
    usleep( 100000 );
  }
}

int Server::get_size()
{
  Glib::Mutex::Lock lock(mutex_);
  return buffers.size();
}


/* this expires buffers if they haven't been updated in a long time,
 * therefore allowing a new source to be added */

void Server::expire()
{
  while(1)
  {
    {
      Glib::Mutex::Lock lock(mutex_);
      time(&currenttime);
      for(int i = 0; i < buffers.size(); i++)
      {
        if( difftime( currenttime, times[i] ) > BUFTIMEOUT )
        {
          cout << "buffer " << i << " will now expire\n";
          delete buffers[i];
          buffers.erase(buffers.begin()+i);
          times.erase(times.begin()+i);
          endpoints.erase(endpoints.begin()+i);
          
          // element i has been deleted, i-- is required
          i--;
        }
      }
    }
    usleep( 1000000 );
  }
}
