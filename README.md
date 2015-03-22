syndilights v1
==============

This is the first iteration of the syndilights (syn2lights) project code.
Very solid (read complex) C++ implementation of the frameserver.

For the current state of the art please see the 'v2' directory.

For historical reasons see this Video for the initial implementation idea: https://www.youtube.com/watch?v=r5T5VG92JsA (Spoken language is Luxembourgish, sorry no translation available but the images speak for themselves)

Install dependencies
--------------------

```
sudo apt-get install libboost-all-dev libncurses5-dev libglibmm-2.4-dev build-essential
```

setup
-----

The system is set up of various parts. See frameserver.png

Latest setup is:

* displayclient/html/ws_udp.js
-----------------------------
A nodejs server => $ node displayclient/html/ws_udp.js
This listens for UDP packets to show on a virtual building
This server creates a webserver where a browser can connect to see a simulation
of the building. => $ firefox/chromium displayclient/html/visionneuse.html

* frameserver
-------------
A c++ program receiving UDP frames combining them to generate the final display

* clients
---------
Written in any language, creates UDP packets which are sent to the frameserver

Building configuration
----------------------

The main configuration of the frameserver is done in <defines.h>

Main tuneables:

```
// where to send the UDP output of the frameserver
#define REMOTE_IP "127.0.0.1"
#define REMOTE_PORT 1234

// description of the frame this server understands
#define HASH "s2l\n8>18> "
// one byte number + 10 character hash plus newline
#define HEADEROFFSET 12

// 12 windows per floor, 8 floors, Value:Alpha
#define WIDTH 12
#define HEIGHT 8
// always has an alpha channel so for rgb it's 3+1=4
#define CHANNELS 4

// 8 segments per window (7 seg plus dot), 12 segments per floor, RGBA
#define SEGNUM 8
#define SEGWIDTH 12
#define SEGCHANNELS 4
```
