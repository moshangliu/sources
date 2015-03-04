CPPFLAGS+=-g -O3 -std=c++11 -Wall -pipe -D_REENTRANT

INCLUDEDIR=../../../include/
SRCDIR = src

INCLUDES=-I../../../include/ \
	-I../../gtest/include \
	-I/opt/soft/tcpsvr/log4cplus/include

DEP_LIBS = /opt/soft/tcpsvr/gcc-4.8.1/lib64/libstdc++.a \
	/opt/soft/tcpsvr/log4cplus/lib/liblog4cplus.a \
	../../gtest/lib/libgtest.a \
	../../gtest/lib/libgtest_main.a \
	../../../output/lib/libserver.a \
	 -lpthread

