version=1.0.0.0
builddate=$(shell date)

OUTDIR=output

CPPFLAGS+=-g -std=c++11 -O3 -Wall -pipe -D_REENTRANT -pthread -DVERSION=\""$(version)"\" -DBUILD_DATE=\""$(builddate)"\"
CC= g++

MODULES = Server Thread Common ConfManager
INCLUDEDIR=../../include/
SRCDIR=src
INCLUDES=-I$(INCLUDEDIR)

%.o: %.cpp $(wildcard  $(INCLUDEDIR)/*.h)
	@$(CC) $(CPPFLAGS) $(INCLUDES) -c $< -o $@
