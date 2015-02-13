#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <gtkmm.h>

#include "Renderer.h"

class AppWindow : public Gtk::Window
{
	public:
		AppWindow();
		virtual ~AppWindow();
			
	protected:
		Gtk::HBox hbox;
		
		Renderer canvas;

	private:

};

#endif
