#include "AppWindow.h"

AppWindow::AppWindow() :
	hbox(),
	canvas()
{
	hbox.pack_start(canvas);
	add(hbox);
	show_all();

	canvas.signal_size_allocate().connect(
		sigc::mem_fun(canvas,&Renderer::on_resize_event));		
  
	resize(420,800);
}

AppWindow::~AppWindow()
{
}
