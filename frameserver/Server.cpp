#include "Server.h"

/* TODO: add ability to force z-value
 * add display of chosen and forced z-value (maybe with differring colours
 * so as to use less space!
 * //*/

Server::Server(int _port )
{
  port = _port;
  displaycounter = 0;
  packetcounter = 0;
  consoleinit = false;
  mode = FRAME;
  selected_buffer=0;
  launch_threads();
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
  threads.push_back( Glib::Thread::create( sigc::mem_fun(this, &Server::send), false ) );
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
  try
  {
    boost::asio::io_service io_service;
     
    // next line is NOT thread-safe because we're accessing "port" without lock
    // however, it is done only once before any other threads are started and
    // should be safe
    udp::socket socket(io_service, udp::endpoint(udp::v4(), port));

    //cout << "listening" << endl;
    frame_t frame;
    while (1)
    {
      // creating the buffer each time is faster than zeroing it out
      boost::array<unsigned char, BUFLEN> recv_buf;
      udp::endpoint remote_endpoint;
      boost::system::error_code error;
      
      socket.receive_from(boost::asio::buffer(recv_buf),
          remote_endpoint, 0, error);


      // DEBUG OUTPUT RAW BUFFER DATA from packet
      //std::cout << recv_buf.data() << std::endl;

      // check whether this is a valid packet and discard it if it isn't
      // note, this is a very hack way of comparing strings...
      if( std::string( (char*)recv_buf.data(), (size_t)10 ) != std::string(HASH) )
        continue;

      // DEBUG send reply to sender
      // socket.send_to( boost::asio::buffer("ACK",3) ,remote_endpoint);

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
          //cout << "adding new buffer for " << remote_endpoint <<  endl;
          buffers.push_back( new Buffer( endpointstring.str() ) );
          endpoints.push_back( remote_endpoint );
          times.push_back( currenttime );
        }
        
        // discard packet, we're not accepting any more sources!
        else if( !known && size >= NUMBUFS )
        {
          //cout << "no more buffers left! " << bufnum << endl;
          continue;
        }
        
        packetcounter++;        
        
        // copy frame information into the buffer
        frame.z = recv_buf[10];
        for(int i = 0; i < HEIGHT; i++)
        {
          for(int j = 0; j < WIDTH; j++)
          {
            for(int a = 0; a < CHANNELS; a++)
            {
              frame.windows[i][j][a] = recv_buf[HEADEROFFSET + i*(CHANNELS*WIDTH+1) + j*CHANNELS + a];
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
      } // lock is released here because the block ends
      
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

/* this sends the frame content to a host expecting udp packets */
void Server::send()
{

  const int length = 12 + HEIGHT*(WIDTH*CHANNELS+1) + SEGWIDTH*(SEGNUM*SEGCHANNELS+1);
  static char data[length];
  
  while(1)
  {
    {
      // lock the object for this operation
      Glib::Mutex::Lock lock(mutex_);
      // write the frame into a char buffer
      for(int i = 0; i < HEIGHT; i++)
      {
        for(int j = 0; j < WIDTH; j++)
        {
          for(int k = 0; k < CHANNELS; k++)
          {
            data[i*(WIDTH*CHANNELS+1) + j*CHANNELS + k] = frame.windows[i][j][k];
          }
        }
        data[i*(WIDTH*CHANNELS+1) + (WIDTH*CHANNELS)] = '\n';
      }
      
      for(int i = 0; i < SEGWIDTH; i++)
      {
        for(int j = 0; j < SEGNUM; j++)
        {
          for(int k = 0; k < SEGCHANNELS; k++)
          {
            data[HEIGHT*(WIDTH*CHANNELS+1) +
                  i*(SEGNUM*SEGCHANNELS+1) + j*SEGCHANNELS + k] = frame.segments[i][j][k];
          }
        }
        data[HEIGHT*(WIDTH*CHANNELS+1) +
                  i*(SEGNUM*SEGCHANNELS+1) + (SEGNUM*SEGCHANNELS)] = '\n';
      }
    }

      try
    {
      boost::asio::io_service io_service;

      /*UDP */
      boost::asio::ip::udp::endpoint u_remote_endpoint( boost::asio::ip::address_v4::from_string((char*)REMOTE_IP) , (short)REMOTE_PORT );
      udp::socket u_socket(io_service, udp::endpoint(udp::v4(),0));
      u_socket.send_to(boost::asio::buffer(data,length), u_remote_endpoint);

      /* TCP */
      boost::asio::ip::tcp::endpoint t_remote_endpoint( boost::asio::ip::address_v4::from_string((char*)REMOTE_IP) , (short)REMOTE_PORT );
      tcp::socket t_socket(io_service);
      t_socket.connect(t_remote_endpoint);
      boost::asio::write(t_socket, boost::asio::buffer(data, length));

    }
    catch (std::exception& e)
    {
      //std::cerr << e.what() << std::endl;
    }
  // sleep until next update cycle
  usleep(25000);
  }
}

/* the framemixer, this periodically (40 times a second) reads all input
   buffers and then produces output, ready to be displayed.
   */
void Server::mix()
{
  int size = 0;
  int counter = 0;
  int pixel = 0;

  int frames = 0;

  while(1)
  {
    frames++;
    frame_t temp_frame;
    float temp_alpha;

    // we lock the buffers for a long time, but we need to make sure
    // that none of the buffers is allowed to expire while we're working on it!
    {
      Glib::Mutex::Lock lock(mutex_);
      displaycounter++;
      size = buffers.size();

			// zero out the frame
			for(int i = 0; i < HEIGHT; i++)
			{
				for(int j = 0; j < WIDTH; j++)
				{
					for(int a = 0; a < CHANNELS; a++)
					{
						frame.windows[i][j][a] = 0;
					}
				}
			}
			
			for(int w = 0; w < SEGWIDTH; w++ )
			{
				for(int n = 0;n < SEGNUM; n++)
				{
					for(int a = 0; a < SEGCHANNELS; a++)
					{
						frame.segments[w][n][a] = 0;
					}
				}
			} // zero out frame


      // implement alpha blending
      for(int x = 0; x < size; x++)
      {
	temp_frame = buffers[x]->get();
        for(int i = 0; i < HEIGHT; i++)
        {
          for(int j = 0; j < WIDTH; j++)
          {
	    temp_alpha = (float)temp_frame.windows[i][j][CHANNELS-1]/255;						
            for(int a = 0; a < CHANNELS; a++)
            {
							// this works for the colors and for the alpha channel
              pixel = (1-temp_alpha)*frame.windows[i][j][a] + temp_alpha*temp_frame.windows[i][j][a];

              //make sure we don't do anything silly
              if( pixel >= 255 )
              	frame.windows[i][j][a] = 255;
              else if( pixel <= 0 )
              	frame.windows[i][j][a] = 0;
              else
              	frame.windows[i][j][a] = pixel;
            }
          }
        }
        
        for(int w = 0; w < SEGWIDTH; w++ )
        {
          for(int n = 0;n < SEGNUM; n++)
          {
						temp_alpha = (float)temp_frame.segments[w][n][SEGCHANNELS-1]/255;
            for(int a = 0; a < SEGCHANNELS; a++)
            {
							// this works for the colors and for the alpha channel
							pixel = (1-temp_alpha)*frame.segments[w][n][a] + temp_alpha*temp_frame.segments[w][n][a];

							// make sure we don't make silly mistakes
							if( pixel >= 255 )
								frame.segments[w][n][a] = 255;
							else if( pixel <= 0 )
								frame.segments[w][n][a] = 0;
							else
								frame.segments[w][n][a] = pixel;
            }
          }
        }
      }
      // temp frame has validity in the loop only so it can be safely used without locking the whole object
      // we do this in case output() takes a long time to return. If it read frame directly, we'd end up
      // waiting for a long time
			temp_frame = frame;
		} /* release lock and send off to hardware
      * (note, temp_frame is passed by value and will live only in the
      * argument of the "output" function and does not need to be locked) */
	output(temp_frame);
  usleep( 25000 );
	}
}

// output to hardware using OLA
void Server::output(frame_t _frame)
{
}

void Server::console()
{
	initscr();
	keypad(stdscr,TRUE);
	cbreak();
	noecho();
    
	if(has_colors() == FALSE)
	{	
		endwin();
		printf("Your terminal does not support color\n");
		// not sure what happens here because of threads!
		exit(1);
	}
	start_color();			/* Start color 			*/

	init_pair(1,COLOR_RED,COLOR_BLACK);
	init_pair(2,COLOR_GREEN,COLOR_BLACK);
	init_pair(3,COLOR_BLUE,COLOR_BLACK);
	init_pair(4,COLOR_BLACK,COLOR_WHITE);

	{
		Glib::Mutex::Lock lock(mutex_);
		consoleinit = true;
		threads.push_back( Glib::Thread::create( sigc::mem_fun(this, &Server::input), false) );
	}

	while(1)
	{
		{
			// we'll be accessing some data to provide statistics, lock the Server!!!
			Glib::Mutex::Lock lock(mutex_);
			mvprintw(0,0,"Clients %d | F2 Frame | F3 Values | F4 Clients | F5 Stats | input: %d            ", buffers.size(),console_input );
			switch(mode)
			{
				case FRAME:
					console_printframe(frame);
					break;
				case FRAME_VALUES:
					console_printframe_values(frame);
					break;
				case CLIENTS:
					console_printclients();
					break;
				default:
					console_printframe(frame);
			}
			refresh();			/* Print it on to the real screen */
		}
		usleep( 20000 );
	}
	endwin();			/* End curses mode		  */
}

void Server::input()
{
  int c;
  
  // get the number of buffers in a threadsafe manner (see Server::get_size() )
  int size = get_size();
  
	while(consoleinit)
	{
		// getch will wait for input, so loop will not lock up cpu
		c = getch();
		{
			switch(c)
      {
        case KEY_F(2):
          mode = FRAME;
          clear();
          break;
        case KEY_F(3):
          mode = FRAME_VALUES;
          clear();
          break;
        case KEY_F(4):
        	mode = CLIENTS;
        	clear();
        	break;
        case KEY_DOWN:
          // get the number of buffers in a threadsafe manner (see Server::get_size() )
          if( mode == CLIENTS && selected_buffer+1 < get_size() )
            selected_buffer++;
          break;
        case KEY_UP:
          if( mode == CLIENTS && selected_buffer-1 >= 0 )
            selected_buffer--;
            break;
        case '0':
        default:
        	{
		Glib::Mutex::Lock lock(mutex_);
          	console_input = c;
          }
      }
		}
	}
}

/* the console functions should only be used in the console thread, they don't
 * implement their own locking and they need ncurses to be initialised */
void Server::console_printframe(frame_t _frame)
{
  int x1,y1,x2,y2;
	// output the current screen contents
	for(int i = 0; i < HEIGHT; i++)
	{
		for(int j = 0; j < WIDTH; j++)
		{
			 mvprintw(i+2,j,"%c", brtoc(_frame.windows[i][j][0]) );
		}
	}

// TODO: correct segment numbering
	
	for(int w = 0; w < SEGWIDTH; w++)
	{
		for(int n = 0; n < SEGNUM; n++)
		{
			// the segments of a display are numbered from bottom to top
			// in a clockwise manner (6'o clock = 0)
      switch(n)
      {
        case 0:
          x1 = 2;
          x2 = 3;
          y1 = 7;
          y2 = 7;
          break;
        case 1:
          x1 = 0;
          x2 = 0;
          y1 = 5;
          y2 = 6;
          break;
        case 2:
          x1 = 2;
          x2 = 3;
          y1 = 4;
          y2 = 4; 
          break;
        case 3:
          x1 = 0;
          x2 = 0;
          y1 = 2;
          y2 = 3;
          break;
        case 4:
          x1 = 2;
          x2 = 3;
          y1 = 1;
          y2 = 1;
          break;
        case 5:
          x1 = 4;
          x2 = 4;
          y1 = 2;
          y2 = 3;
          break;
        case 6:
          x1 = 4;
          x2 = 4;
          y1 = 5;
          y2 = 6;
          break;
        case 7:
          x1 = 5;
          x2 = 5;
          y1 = 7;
          y2 = 7;
          break;
      } // switch
			
      for(int a = 0; a < SEGCHANNELS-1; a++)
			{
        attron(COLOR_PAIR(a+1));
				mvprintw( HEIGHT+2+a*8+y1,
                  w*7+x1,
                  "%c",brtoc(_frame.segments[w][n][a]));
        mvprintw( HEIGHT+2+a*8+y2,
                  w*7+x2,
                  "%c",brtoc(_frame.segments[w][n][a]));
        attroff(COLOR_PAIR(a+1));
        
			}
		}
	}		
}

void Server::console_printframe_values(frame_t _frame)
{
	// output the current screen contents
	for(int i = 0; i < HEIGHT; i++)
	{
		for(int j = 0; j < WIDTH; j++)
		{
			 mvprintw(i*2+2,j*4," %d ", brtoi(_frame.windows[i][j][0]) );
		}
	}
}

// print the list of clients connected to the server
void Server::console_printclients()
{
	int rows, cols, offset=0;
	getmaxyx(stdscr,rows,cols);
  clrtobot();
	for(int i = 0; i < buffers.size(); i++)
		{
    // rows-2 because there is a header
		if(i > 0 && i%(rows-2)==0)
			offset += 32;
		if( offset + 32 < cols )
      {
      //set black on white if the current buffer is selected in the screenview
      
      if( i == selected_buffer )
        attron(COLOR_PAIR(4));
        
			mvprintw(i%(rows-2)+2,offset,"(%3d)[%3d] %s\n", i,buffers[i]->get().z,buffers[i]->get_id().c_str() );
      
      if( i == selected_buffer )
        attroff(COLOR_PAIR(4));
      }
		}
}

void Server::console_printstats()
{
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
          //cout << "buffer " << i << " will now expire\n";
          delete buffers[i];
          buffers.erase(buffers.begin()+i);
          times.erase(times.begin()+i);
          endpoints.erase(endpoints.begin()+i);
          
          // element i has been erased, i-- is required
          i--;
        }
      }
    }
    usleep( 1000000 );
  }
}

int Server::brtoi(unsigned char br)
{
  return (int)br; 
}

// converts a given brightness value (0-255) to a character
// relatively efficiently
char Server::brtoc(unsigned char br)
{
  static char chars[11] = " ._-~:+*#@";
  if(br > 124)
  {
    if( br > 202 )
    {
      if( br > 228 )
        return chars[9];
      else
        return chars[8];
    }
    else
    {
      if( br > 176 )
        return chars[7];
      else
        return chars[6];
    }
  }
  else
  {
    if( br > 72 )
    {
      if( br > 124 )
        return chars[5];
      if( br > 98 )
        return chars[4];
      else
        return chars[3];
    }
    else
    {
      if( br > 46 )
        return chars[2];
      if( br > 20 )
        return chars[1];
      else
        return chars[0];
    }
  }
}

// implementation dependent behaviour of unsigned char?!
void Server::test()
{
  char i = 0;
  unsigned char j = 0;
  while(1)
  {
    cout << int(i) << endl;
    cout << int(j) << endl;
    i--;
    j = i;
    usleep(20000);
  }
}
