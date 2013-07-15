frameserver:
	g++ -O2 -ggdb -lncurses `pkg-config --libs --cflags glibmm-2.4` `pkg-config --libs --cflags gthread-2.0` -lboost_system -o frameserver Buffer.cpp Buffers.cpp Server.cpp frameserver.cpp
test:
	g++ -O2 -pipe -fomit-frame-pointer `pkg-config --libs --cflags glibmm-2.4` `pkg-config --libs --cflags gthread-2.0`  -o test Buffer.cpp Buffers.cpp test.cpp
