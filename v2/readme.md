syndilights v2
==============

This is a Python3 implementation for the syndilights project.
Nb: The Processing Python code is Python2

This readme assumes you are in the 'v2' directory.

Flow diagram
============

    +----------------+
    |                |
    |  processing    |
    |  jton(python2) |
    |                |
    +--------+-------+
             |   TCP     
             v        
    +--------+-------+
    |                |
    |  reciever      |
    |  python3       |
    +--------+-------+
             |   TCP     
             v   6379     
    +--------+-------+
    |                |
    |    redis       |
    |                |
    +--------+-------+                            
             |  TCP                               
             v                             o-o o o-o o o-o o
    +--------+-------+                     | | | | | | | | |
    |                |                     o o o o o o o o o
    |    forwarder   |                     | | | | | | | | |
    |    python3     |                     o o o o o o o o o
    |                |                     | | | | | | | | |
    +--------+-------+                     o o o o o o o o o
             |  serial                     | | | | | | | | |
             v                             o o-o o o-o o o-o
    +--------+-------+  +--------+ --------+     |     |       
    |                |==|        | --------------+     |
    |    teensy3.1   |==| OctoWS | --------------------+
    |                |==|        |
    +----------------+  +--------+

Installation
============

```
    # This is for install on a linux64 bit. Your milage may vary
    # download the processinag archive here https://processing.org/download/
    tar xvzf processing-*-linux64.tgz
    cd processing-*
    alias processing=$PWD/processing
    echo "alias processing=$PWD/processing" >> .profile
    # install python modules
    sudo apt-get install python3-pip redis-server python-virtualenv
    virtualenv -p /usr/bin/python3 virtenv
    . virtenv/bin/activate
    pip install -r requirements.txt
    processing
    # Java->Add mode... [Python] -> Install...
```

Base Setup
==========


Make sure redis-server is running. If it is you should be able to connect to port 6379

```
$ telnet localhost 6379
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'
```

If not this might happen and you have to launch redis-server:

```
$ telnet localhost 6379
Trying 127.0.0.1...

telnet: connect to address 127.0.0.1: Connection refused
telnet: Unable to connect to remote host
$ redis-server
[74407] 22 Mar 11:47:33.713 # Warning: no config file specified, using the default config. In order to specify a config file use redis-server /path/to/redis.conf
[74407] 22 Mar 11:47:33.714 * Increased maximum number of open files to 10032 (it was originally set to 2560).
                _._
           _.-``__ ''-._
      _.-``    `.  `_.  ''-._           Redis 2.8.19 (00000000/0) 64 bit
  .-`` .-```.  ```\/    _.,_ ''-._
 (    '      ,       .-`  | `,    )     Running in stand alone mode
 |`-._`-...-` __...-.``-._|'` _.-'|     Port: 6379
 |    `-._   `._    /     _.-'    |     PID: 74407
  `-._    `-._  `-./  _.-'    _.-'
 |`-._`-._    `-.__.-'    _.-'_.-'|
 |    `-._`-._        _.-'_.-'    |           http://redis.io
  `-._    `-._`-.__.-'_.-'    _.-'
 |`-._`-._    `-.__.-'    _.-'_.-'|
 |    `-._`-._        _.-'_.-'    |
  `-._    `-._`-.__.-'_.-'    _.-'
      `-._    `-.__.-'    _.-'
          `-._        _.-'
              `-.__.-'

[74407] 22 Mar 11:47:33.715 # Server started, Redis version 2.8.19
[74407] 22 Mar 11:47:33.715 * The server is now ready to accept connections on port 6379
```

Run the listener receiving the images from the clients:

```
    python3 backend/forwarding/receiver.py
```

Run the forwarder that talks to the teensy via Serial and make sure to adapt the 'serialConfigure' option in the file:

```
    python3 backend/forwarding/forward.py
```


Processing
==========

To use syndilights with processing you need to push the following code onto the Teensy 3.1:

```
    backend/arduino/VideoDisplayTeensy31/VideoDisplayTeensy31.ino
```

/!\ When running the teensdyduino installer over your Arduino (tested on 1.0.6) installation make sure NO libraries are selected (yes, NOT even the WS2811) /!\

Open processing and add a new Mode. This can be achieved by clicking on 'Java' -> Add Mode…
In the mode manager select Python and install

/!\ Restart processing /!\

Next step will be to open the following file in processing:

```
    processing backend/processing/PixelControl_TCP/PixelControl_TCP.pyde
```

Configure the height and width of your Pixel Setup
A frame rate of 30 is good enough for most uses
For test purposes the brightness has been set to low

The current demo is a green dot that will chase around the LED-strip.

Setting long_line to True or False will change the wrapping mode

long_line=False
```
             o-+ o-+ o-+ o-+ o
             | | | | | | | | |
             o | o | o | o | o
             | | | | | | | | |
             o | o | o | o | o
             | | | | | | | | |
             o | o | o | o | o
             | | | | | | | | |
             o +-o +-o +-o +-o
```
long_line=True
```
             o---o   o---o   o
             |   |   |   |   |
             o   o   o   o   o
             |   |   |   |   |
             o   o   o   o   o
             |   |   |   |   |
             o   o   o   o   o
             |   |   | | |   |
             o   o---o   o---o
```

Sending images to the grid
==========================

<section needs revision>

Run the script merging the images:

```
    python3 png_merger.py
```

Run the scripts sending the images:

```
    python3 png_creator.py
```
