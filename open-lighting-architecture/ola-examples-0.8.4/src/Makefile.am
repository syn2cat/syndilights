
AM_CXXFLAGS = -Wall -Werror $(libola_CFLAGS)
AM_LDFLAGS =  $(libola_LIBS)

if HAVE_NCURSES
NCURSES_PROGS = ola_dmxconsole ola_dmxmonitor
ola_dmxconsole_SOURCES = ola-dmxconsole.cpp
ola_dmxconsole_LDADD = @LIBS@ -lcurses
ola_dmxmonitor_SOURCES = ola-dmxmonitor.cpp
ola_dmxmonitor_LDADD = @LIBS@ -lcurses
endif

bin_PROGRAMS = ola_dev_info ola_usbpro ola_artnet ola_e131 \
               ola_streaming_client ola_rdm_discover ola_rdm_get \
               $(NCURSES_PROGS)

ola_artnet_SOURCES = ola-artnet.cpp
ola_artnet_LDADD = @LIBS@ -lolaartnetconf libolaconfig.la
ola_dev_info_SOURCES = ola-client.cpp
ola_e131_SOURCES = ola-e131.cpp
ola_e131_LDADD = @LIBS@ -lolae131conf libolaconfig.la
ola_streaming_client_SOURCES = ola-streaming-client.cpp
ola_usbpro_SOURCES = ola-usbpro.cpp
ola_usbpro_LDADD = @LIBS@ -lolausbproconf libolaconfig.la
ola_rdm_get_SOURCES = RDMController.cpp RDMHandler.cpp ola-rdm.cpp
ola_rdm_discover_SOURCES = ola-rdm-discover.cpp

noinst_PROGRAMS = ola_throughput
ola_throughput_SOURCES = ola-throughput.cpp
ola_throughput_LDADD = @LIBS@

noinst_LTLIBRARIES = libolaconfig.la
libolaconfig_la_SOURCES = OlaConfigurator.cpp

EXTRA_DIST = OlaConfigurator.h RDMController.h RDMHandler.h

install-exec-hook:
	$(LN_S) -f $(bindir)/ola_dev_info $(DESTDIR)$(bindir)/ola_plugin_info
	$(LN_S) -f $(bindir)/ola_dev_info $(DESTDIR)$(bindir)/ola_uni_info
	$(LN_S) -f $(bindir)/ola_dev_info $(DESTDIR)$(bindir)/ola_patch
	$(LN_S) -f $(bindir)/ola_dev_info $(DESTDIR)$(bindir)/ola_uni_name
	$(LN_S) -f $(bindir)/ola_dev_info $(DESTDIR)$(bindir)/ola_uni_merge
	$(LN_S) -f $(bindir)/ola_dev_info $(DESTDIR)$(bindir)/ola_set_dmx
	$(LN_S) -f $(bindir)/ola_dev_info $(DESTDIR)$(bindir)/ola_set_priority
	$(LN_S) -f $(bindir)/ola_rdm_get $(DESTDIR)$(bindir)/ola_rdm_set
