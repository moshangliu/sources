version=1.0.0.0
builddate=$(shell date)

# �������Ŀ¼
OUTDIR=output

# �������ѡ��
CPPFLAGS+=-g -O3 -Wall -pipe -D_REENTRANT -pthread -DVERSION=\""$(version)"\" -DBUILD_DATE=\""$(builddate)"\"
CC= g++

MODULES = HttpServer Thread Common ConfManager
INCLUDEDIR=../../include/
SRCDIR = src
INCLUDES=-I$(INCLUDEDIR)

%.o: %.cpp $(wildcard  $(INCLUDEDIR)/*.h)
	@$(CC) $(CPPFLAGS) $(INCLUDES) -c $< -o $@