frameserver:
	g++ -O2 -ggdb Buffer.cpp Buffers.cpp Server.cpp frameserver.cpp `pkg-config --libs --cflags ncurses` `pkg-config --libs --cflags glibmm-2.4` `pkg-config --libs --cflags gthread-2.0` -lboost_system -o frameserver
test:
	g++ -O2 -pipe -fpermissive -fomit-frame-pointer Buffer.cpp Buffers.cpp test.cpp `pkg-config --libs --cflags glibmm-2.4` `pkg-config --libs --cflags gthread-2.0`  -o test 
thread_test:
	g++ -O2 -pipe -fomit-frame-pointer thread.cc -o thread `pkg-config --cflags --libs glibmm-2.4`
