version=1.0.0.0
builddate=$(shell date)

OUTDIR=output

CPPFLAGS+=-g -O3 -Wall -pipe -D_REENTRANT -pthread -DVERSION=\""$(version)"\" -DBUILD_DATE=\""$(builddate)"\"
CC= g++

SRCDIR = src
MODULES = Thread Common Main UDP

INCLUDEDIR=../../include/
LOG4CPLUS_INCLUDE_DIR=../../third_party/log4cplus-1.1.2/include/
LOG4CPLUS_LIB_DIR=../../third_party/log4cplus-1.1.2/lib/

INCLUDE_DIRS=-I$(INCLUDEDIR) -I$(LOG4CPLUS_INCLUDE_DIR)
LIBS_DIR=-L$(LOG4CPLUS_LIB_DIR)
LIBS=-llog4cplus

CC_COMPILE_LINE=$(CPPFLAGS) $(INCLUDE_DIRS) $(LIBS_DIR) $(LIBS)

%.o: %.cpp $(wildcard  $(INCLUDEDIR)/*.h)
	@$(CC) $(CC_COMPILE_LINE) -c $< -o $@
