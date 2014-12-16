include Makefile.d


INCPATH=$(INCLUDES)
DEP_LDFLAGS=$(LIBPATH)
DEP_LDLIBS=$(DEP_LIBS)
CXX=g++

PATTERN=test*.cpp
TESTFILES=$(wildcard $(PATTERN))
EXE=$(basename $(TESTFILES))
all : $(EXE)
$(EXE) : % : %.cpp
	$(CXX) $^ -o $@ $(CXXFLAGS) $(CFLAGS) $(INCPATH) $(DEP_LDFLAGS) $(DEP_LDLIBS)
.PHONY: list clean
list:
	@echo $(EXE)
clean:
	rm -f $(EXE)



