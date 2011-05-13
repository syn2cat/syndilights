
#include "Renderer.h"


#include <iostream>

Renderer::Renderer()
{
  // this determines the refresh rate
	Glib::signal_timeout().connect( sigc::mem_fun(*this, &Renderer::on_timeout), 10 );


  // drawing in GTK+ is done in the "expose_event" signal handler
	#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
	//Connect the signal handler if it isn't already a virtual method override:
	signal_expose_event().connect(sigc::mem_fun(*this, &Renderer::on_expose_event), false);
	#endif //GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED

	thickness = (int)((double)get_width()*0.05);

	fullscreen = true;
	invalid.xmin = get_width();
	invalid.ymin = get_height();
	invalid.width = invalid.height = 0;

  // launch the udp listener
  Glib::Thread::create( sigc::mem_fun(this, &Renderer::listen), false);
}

Renderer::~Renderer()
{}

void Renderer::on_resize_event(Gtk::Allocation &allocation)
{
	// thickness controls the area that is redrawn around a new point
	thickness = (int)((double)get_width()*0.05);
	
	// if fullscreen redraw is enabled set redraw limits to drawing area size
	if( fullscreen )
	{
		invalid.xmin = 0;
		invalid.width = get_width();
		invalid.ymin = 0;
		invalid.height = get_height();
	}
}

bool Renderer::on_expose_event(GdkEventExpose *event)
{
  frame_t f;
  // read the frame in a thread-safe manner
  {
      Glib::Mutex::Lock lock(mutex_);
      f = frame;
  }
	Glib::RefPtr<Gdk::Window> window = get_window();

	if(window)
	{
		//Gtk::Allocation allocation = get_allocation();
		width = get_width();
		height = get_height();
		Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
		cr->set_line_cap(Cairo::LINE_CAP_ROUND);
		cr->set_line_join(Cairo::LINE_JOIN_ROUND);

		
		if(event)
		{
				cr->rectangle(event->area.x, event->area.y,
								event->area.width, event->area.height);
				cr->clip();
		}
		
    // rescale coordinates of the drawing context to 1.0 1.0
		cr->scale(width,height);
		
    // fill the backdrop
		cr->set_source_rgb(0.3,0.3,0.3);
		cr->paint();

    cr->set_line_width(0.05);

    // geometry of the building face
    double window_width = 0.04;
    double window_height = 0.08;
    double window_hsep = 0.03;
    double window_vsep = 0.04;

    // draw the windows
    for(int i = 0; i < HEIGHT; i++)
    {
      for(int j = 0; j < WIDTH; j++)
      {
        cr->set_source_rgb( (double)f.windows[i][j][0]/255, (double)f.windows[i][j][0]/255, (double)f.windows[i][j][0]/255 );
        cr->rectangle( window_hsep + j*(window_hsep+window_width) , window_vsep + i*(window_height+window_vsep), window_width, window_height );
        cr->fill();
      }
    }
    
    // draw the segments
    for(int w = 0; w < SEGWIDTH; w++ )
    {
      for(int n = 0;n < SEGNUM; n++)
      {
        double r = (double)f.segments[w][n][0]/255;
        double b = (double)f.segments[w][n][1]/255;
        double g = (double)f.segments[w][n][2]/255;

        switch(n)
        {
          case 0:
             draw_hsegment( window_hsep + w*(window_width+window_hsep), 0.88 , 0.01, window_width, r, g, b, cr);
              break;
          case 3:
              draw_hsegment( window_hsep + w*(window_width+window_hsep), 0.88 + window_height , 0.01, window_width, r, g, b, cr);
              break;
          case 6:
            draw_hsegment( window_hsep + w*(window_width+window_hsep), 0.88 + window_height/2, 0.01, window_width, r, g, b, cr);
            break;
          case 1:
            draw_vsegment( window_hsep + w*(window_width+window_hsep) + window_width, 0.88, 0.01, window_width, r,g,b,cr);
            break;
          case 2:
            draw_vsegment( window_hsep + w*(window_width+window_hsep) + window_width, 0.88 + window_width, 0.01, window_width, r,g,b,cr);
            break;
          case 4:
            draw_vsegment( window_hsep + w*(window_width+window_hsep), 0.88 + window_width, 0.01, window_width, r,g,b,cr);
            break;
          case 5:
            draw_vsegment( window_hsep + w*(window_width+window_hsep), 0.88, 0.01, window_width, r,g,b,cr);
            break;
          case 7:
            cr->arc( window_hsep + w*(window_width+window_hsep) + window_width + 0.005, 0.88 + window_height + 0.002, 0.003, 0 , 2*PI );
            cr->set_source_rgb(r,g,b);
            cr->fill();
            break;
          default:
            break;
        }
      }
    }
  }	
	return true;
}

// x, y, thickness, length, colours
void Renderer::draw_hsegment(double x, double y, double t, double l, double r, double g, double b, Cairo::RefPtr<Cairo::Context> cr)
{
  cr->move_to(x,y);
  cr->line_to(x+0.1*l,y+0.5*t);
  cr->line_to(x+0.9*l,y+0.5*t);
  cr->line_to(x+l,y);
  cr->line_to(x+l-0.1*l,y-0.5*t);
  cr->line_to(x+l-0.9*l,y-0.5*t);
  cr->close_path();
  cr->set_source_rgb(r,g,b);
  cr->fill();
}

// x, y, thickness, length, colours
void Renderer::draw_vsegment(double x, double y, double t, double l, double r, double g, double b, Cairo::RefPtr<Cairo::Context> cr)
{
  cr->move_to(x,y);
  cr->line_to(x+0.5*t,y+0.1*l);
  cr->line_to(x+0.5*t,y+0.9*l);
  cr->line_to(x,y+l);
  cr->line_to(x-0.5*t,y+l-0.1*l);
  cr->line_to(x-0.5*t,y+l-0.9*l);
  cr->close_path();
  cr->set_source_rgb(r,g,b);
  cr->fill();
}

// this signal handler tells gtk+ to redraw the window contents
bool Renderer::on_timeout()
{
	/*static*/ Glib::RefPtr<Gdk::Window> win;
	win = get_window();
	if(win)
	{
		Gdk::Rectangle r(invalid.xmin,invalid.ymin,invalid.width,
							 invalid.height);
		win->invalidate_rect(r,false);
	}
	
	// if fullscreen redraw is DISABLED, reset redraw rectangle
	if( !fullscreen )
		{
		invalid.xmin = get_width();
		invalid.ymin = get_height();
		invalid.width = invalid.height = 0;
		}
	
	return true;
}

// listen to udp packets on port 1234 which contain information about the frame
void Renderer::listen()
{
  try
  {
    boost::asio::io_service io_service;
    udp::socket socket(io_service, udp::endpoint(udp::v4(), 1234));

    while (1)
    {
      // creating the buffer each time is faster than zeroing it out
      boost::array<unsigned char, WIDTH*HEIGHT*CHANNELS + SEGWIDTH*SEGNUM*SEGCHANNELS> recv_buf;
      udp::endpoint remote_endpoint;
      boost::system::error_code error;
      
      socket.receive_from(boost::asio::buffer(recv_buf),
          remote_endpoint, 0, error);

      {
        Glib::Mutex::Lock lock(mutex_);
        for(int i = 0; i < HEIGHT; i++)
        {
          for(int j = 0; j < WIDTH; j++)
          {
            for(int a = 0; a < CHANNELS; a++)
            {
              frame.windows[i][j][a] = recv_buf[i*(CHANNELS*WIDTH) + j*CHANNELS + a];
            }
          }
        }
        
        for(int w = 0; w < SEGWIDTH; w++ )
        {
          for(int n = 0;n < SEGNUM; n++)
          {
            for(int a = 0; a < SEGCHANNELS; a++)
            {
              frame.segments[w][n][a] = recv_buf[WIDTH*HEIGHT*CHANNELS + w*(SEGCHANNELS*SEGNUM) + n*SEGCHANNELS + a];
            }
          }
        }
      } // lock is released here because we will not access object-wide resources anymore
      
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
