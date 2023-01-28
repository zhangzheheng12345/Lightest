# Old School makefile for people who don't use CMake for reasons
#
# invoke:
#    make [-f Makefile] : make default target
#    make all           : make target all, which is the default target here and points to "unittest"
#    make unittest      : make target "unittest" demo samples
#    make remake   	    : same as "make clean all"
#    make clean    	    : clean up result + intermediate files
#    make check		    : run unit test demos
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
Test_AllGood     := test_success.exe
Test_OneFail     := test_failing_1.exe
Test_SomeFail    := test_failing_more.exe

INC_PATHS ?= ./include
##############################################################
Test_AllGood_SRC := test/$(Test_AllGood:.exe=.cpp)
Test_OneFail_SRC := test/$(Test_OneFail:.exe=.cpp)
Test_SomeFail_SRC:= test/$(Test_SomeFail:.exe=.cpp)
#
Test_AllGood_OBJS := $(Test_AllGood_SRC:.cpp=.o)
Test_OneFail_OBJS := $(Test_OneFail_SRC:.cpp=.o)
Test_SomeFail_OBJS:= $(Test_SomeFail_SRC:.cpp=.o)

# --- auto assigning inc paths (space separated) to appropriate list of -I directives
#
INCSEARCH := $(foreach d, $(INC_PATHS), -I$d)

%.exe: test/%.o
	@echo "compiling: $(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@"
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@


%.o: %.cpp
	@echo "compiling and linking $< to $@   by invoking:"
	$(CXX) $(CXXFLAGS) -o $@ $(INCSEARCH) -c $<

unittest: $(Test_AllGood) $(Test_OneFail) $(Test_SomeFail)

remake: clean all

clean:
	rm -f $(Test_AllGood) $(Test_AllGood_OBJS) $(Test_OneFail) $(Test_OneFail_OBJS) $(Test_SomeFail) $(Test_SomeFail_OBJS)

info:
	@echo "compiling $(Test_AllGood_SRC) to get result: $(Test_AllGood)"
	@echo "known Makefile arguments are:"
	@echo "all  		build all (only unittests this time)"
	@echo "unittest 	build only the Unittest executables"
	@echo "remake   	clean + all"
	@echo "clean    	clean up result + intermediate files"
	@echo "check		run tests"
	@echo "info 		this one"

check: clean testing

testing: unittest
	chmod +x $(Test_AllGood) $(Test_OneFail) $(Test_SomeFail)
	@ ./$(Test_AllGood)  --no-color && echo "unit tests 1 OK." && \
	! ./$(Test_OneFail)  --no-color && echo "unit tests 2 OK." && \
	! ./$(Test_SomeFail) --no-color && echo "unit tests 3 OK." && \
	 echo "== ALL UNIT TESTS WENT FINE ==" || echo "SOME UNIT TESTs FAILED!"
