version=1.0.0.0
builddate=$(shell date)

OUTDIR=output

#CPPFLAGS+=-g -O3 -std=c++11 -Wall -pipe -D_REENTRANT -DVERSION=\""$(version)"\" -DBUILD_DATE=\""$(builddate)"\"
CPPFLAGS+=-gdwarf-2 -g -std=c++11 -Wall -pipe -D_REENTRANT -DVERSION=\""$(version)"\" -DBUILD_DATE=\""$(builddate)"\"
#CC= g++
CC=/opt/soft/tcpsvr/gcc-4.8.1/bin/g++

SRCDIR = src
MODULES = Thread Common Main UDP Log

INCLUDEDIR=../../include/

INCLUDE_DIRS=-I$(INCLUDEDIR) \
	 -I/opt/soft/tcpsvr/log4cplus/include \
	 -I/opt/soft/tcpsvr/boost/include
LIBS_DIR=/opt/soft/tcpsvr/gcc-4.8.1/lib64/libstdc++.a \
	 /opt/soft/tcpsvr/log4cplus/lib/liblog4cplus.a\
	 -lpthread

CC_COMPILE_LINE=$(CPPFLAGS) $(INCLUDE_DIRS) $(LIBS_DIR)

%.o: %.cpp $(wildcard  $(INCLUDEDIR)/*.h)
	@$(CC) $(CPPFLAGS) $(INCLUDE_DIRS) -c $< -o $@
