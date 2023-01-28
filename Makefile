# Old School makefile for people who don't use CMake for reasons
#
# invoke:
#    make [-f Makefile] : make default target
#    make all           : make target all, which is the default target here and points to "unittest"
#    make unittest      : make target "unittest" demo sample
#    make remake   	    : same as "make clean all"
#    make clean    	    : clean up result + intermediate files
#    make check		    : run unit test demo
#    make info 		    : builtin help
#
# build with some scpecial defines and run GOOD demo:
#    CXXFLAGS="-DDEBUG=0 -DFAIL=0" make clean check
#
# build with some scpecial defines and run FAIL demo:
#    CXXFLAGS="-DDEBUG=0 -DFAIL=1" make clean check
#
.PHONY: all unittest remake clean check info

# 1st target is the target we build with no args, so i want to have 'all' here.
all: unittest

# because "test" is a very bad idea for a test program on unixoid systems, we create another executable
MyUnitTExe     := test_run
MyUnitTExe_SRC := test/test.cpp

INC_PATHS ?= ./include
##############################################################
MyUnitTExe_OBJS := $(MyUnitTExe_SRC:.cpp=.o)

# --- auto assigning inc paths (space separated) to appropriate list of -I directives
#
INCSEARCH := $(foreach d, $(INC_PATHS), -I$d)

$(MyUnitTExe): $(MyUnitTExe_OBJS) $(MyLIB)
	@echo "unit: MyUnitTExe     =$(MyUnitTExe)"
	@echo "unit: MyUnitTExe_OBJS=$(MyUnitTExe_OBJS)"
	@echo "unit: linking $^ (and $(strip $(LIB_LINK))) to $@ by invoking:"
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

%.o: %.cpp
#	@echo "compiling and linking $< to $@   by invoking:"
	$(CXX) $(CXXFLAGS) -o $@ $(INCSEARCH) -c $<

unittest: $(MyUnitTExe)

remake: clean all

clean:
	rm -f $(MyUnitTExe) $(MyUnitTExe_OBJS)

info:
	@echo "compiling $(MyUnitTExe_SRC) to get result: $(MyUnitTExe)"
	@echo "known Makefile arguments are:"
	@echo "all  		build all (only unittest this time)"
	@echo "unittest 	build only the Unittest executable"
	@echo "remake   	clean + all"
	@echo "clean    	clean up result + intermediate files"
	@echo "check		run tests"
	@echo "info 		this one"

check: clean unittest
	chmod +x $(MyUnitTExe)
	./$(MyUnitTExe) && echo "unit tests OK." || echo "UNIT TESTS FAILED!"
