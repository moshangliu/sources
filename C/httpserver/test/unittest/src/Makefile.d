CPPFLAGS+=-g -O3 -Wall -pipe -D_REENTRANT -pthread

MODULES = HttpServer Thread
INCLUDEDIR=../../../include/
SRCDIR = src
UT_BASE_PATH = ../..

INCLUDES=-I$(INCLUDEDIR) \
		 -I$(UT_BASE_PATH)/gtest/include \

LIBPATH = -L$(UT_BASE_PATH)/gtest/lib \
		  -L../../../output/lib/ \

DEP_LIBS = -lgtest \
		   -lgtest_main \
		   -lserver \
		   -lpthread \

