include $(top_srcdir)/common.mk

libdir = $(plugindir)

EXTRA_DIST = ShowNetPlugin.h ShowNetDevice.h ShowNetPort.h ShowNetPackets.h \
             ShowNetNode.h

lib_LTLIBRARIES = libolashownet.la
libolashownet_la_SOURCES = ShowNetPlugin.cpp ShowNetDevice.cpp ShowNetPort.cpp\
                           ShowNetNode.cpp
libolashownet_la_LIBADD = @LIBS@

# Test Programs
TESTS = ShowNetTester
check_PROGRAMS = $(TESTS)
ShowNetTester_SOURCES = ShowNetTester.cpp \
                        ShowNetNode.cpp ShowNetNodeTest.cpp
ShowNetTester_CXXFLAGS = $(CPPUNIT_CFLAGS)
ShowNetTester_LDFLAGS = $(CPPUNIT_LIBS)
ShowNetTester_LDADD = @LIBS@ -lprotobuf ../../common/libolacommon.la
