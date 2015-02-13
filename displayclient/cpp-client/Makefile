displayclient:
	g++ -O2 -march=pentium-m -pipe -fomit-frame-pointer -lboost_system `pkg-config --libs --cflags gdkmm-2.4` `pkg-config --libs --cflags gtkmm-2.4` `pkg-config --libs --cflags gthread-2.0`  -o displayclient Renderer.cpp AppWindow.cpp main.cpp
