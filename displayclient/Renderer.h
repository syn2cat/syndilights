#ifndef RENDERER_H
#define RENDERER_H

#include <gtkmm.h>
#include <cairomm/cairomm.h>
#include <glibmm.h>

#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "defines.h"

using boost::asio::ip::udp;
using boost::asio::ip::tcp;
using namespace std;

class Renderer : public Gtk::DrawingArea
{
	public:
		Renderer();
		virtual ~Renderer();
		
		void on_resize_event(Gtk::Allocation &allocation);
	
	protected:
		
		virtual bool on_expose_event(GdkEventExpose *event);
		bool on_timeout();
		

	private:
    void listen();

    void draw_hsegment(double,double,double,double,double,double,double,Cairo::RefPtr<Cairo::Context>);
    void draw_vsegment(double,double,double,double,double,double,double,Cairo::RefPtr<Cairo::Context>);
  
    Glib::Mutex mutex_;
    frame_t frame;
      
		std::list<position> points;
		std::list<position>::iterator it_points_end;
	
		bool fullscreen;
		int width,height;
		
		area invalid;
		int thickness;
};

#endif
