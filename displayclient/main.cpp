#include <gtkmm.h>
#include <iostream>

#include "defines.h"

#include "AppWindow.h"


int main(int argc, char** argv)
{
	Glib::thread_init();
	std::cout << "init\n";
	Gtk::Main kit(&argc,&argv);
	std::cout << "kit\n";
	AppWindow myWindow;
	std::cout << "window\n";
	
	// launch the udp listener thread in the window in an idle moment
	std::cout << "launch\n";	
	Gtk::Main::run(myWindow);
	std::cout << "run\n";
	return 0;
}
