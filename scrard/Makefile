CPPDEPS = -MT$@ -MF`echo $@ | sed -e 's,\.o$$,.d,'` -MD -MP
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
CXXFLAGS = -std=gnu++0x -Wno-attributes -Wno-deprecated-declarations -Wno-placement-new -I. -I${YBOO}/include `wx-config --cxxflags` -c
LDFLAGS = `wx-config --libs std,adv,awx,things,plotctrl` -Wl,-rpath=${YWX}/lib -Wl,-rpath=${YBOO}/lib -L${YBOO}/lib -lboost_system -lboost_thread

ifneq (,$(findstring MINGW,$(shell uname)))
	PROG = ./bin/Release/$(notdir $(CURDIR)).exe
	CXXFLAGS += -D_WIN32_WINNT=0x0501 
	LDFLAGS += -s -static-libgcc -static-libstdc++ -lwsock32 -lws2_32
else
	PROG = ./bin/Release/$(notdir $(CURDIR))
endif

### Targets: ###
Release: $(PROG)

### Compile: ###
.cpp.o:
	$(CXX) $(CXXFLAGS) -o $@ $(CPPDEPS) $<

### Link: ###
$(PROG): $(OBJECTS)
	@[ -d ./bin/Release ] || mkdir -p ./bin/Release
	g++ -o $(PROG) *.o $(LDFLAGS)

### Clean: ###
cleanRelease:
	@find . -name "*.d" -delete; find . -name "*.o" -delete; [ ! -f $(PROG) ] || rm $(PROG)

clean: cleanRelease
### Dependencies tracking: ###
-include ./*.d


