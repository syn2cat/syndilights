include $(top_srcdir)/common.mk

noinst_LTLIBRARIES = libstreamrpcchannel.la
libstreamrpcchannel_la_SOURCES = StreamRpcChannel.cpp SimpleRpcController.cpp
nodist_libstreamrpcchannel_la_SOURCES = Rpc.pb.cc

EXTRA_DIST = Rpc.proto TestService.proto SimpleRpcController.h \
             StreamRpcChannel.h

BUILT_SOURCES = Rpc.pb.cc Rpc.pb.h TestService.pb.cc TestService.pb.h

Rpc.pb.cc Rpc.pb.h: Rpc.proto
	$(PROTOC) --cpp_out ./ Rpc.proto

TestService.pb.cc TestService.pb.h: TestService.proto
	$(PROTOC) --cpp_out ./ TestService.proto

TESTS = RpcTester
check_PROGRAMS = $(TESTS)
RpcTester_SOURCES = RpcTester.cpp RpcControllerTest.cpp\
                    StreamRpcChannelTest.cpp StreamRpcHeaderTest.cpp
nodist_RpcTester_SOURCES = TestService.pb.cc
RpcTester_CXXFLAGS = $(CPPUNIT_CFLAGS)
RpcTester_LDFLAGS = $(CPPUNIT_LIBS) -lprotobuf
RpcTester_LDADD = ./libstreamrpcchannel.la \
                  ../export_map/libolaexportmap.la \
                  ../logging/liblogging.la \
                  ../network/libolanetwork.la \
                  ../utils/libolautils.la

clean-local:
	rm -f *.pb.{h,cc}

# this has the problem that it doesn't get rebuilt if a dependency changes.
../network/libnetwork.la:
	cd ../network && $(MAKE) $(AM_MAKEFLAGS) libnetwork.la
