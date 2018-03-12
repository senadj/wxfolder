PROG = ./bin/Release/$(notdir $(CURDIR)).exe
CPPDEPS = -MT$@ -MF`echo $@ | sed -e 's,\.o$$,.d,'` -MD -MP
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
CXXFLAGS = -std=gnu++0x -D_WIN32_WINNT=0x0501 -Wno-deprecated-declarations -I. -I${YBOO}/include `wx-config --cxxflags` -c
LDFLAGS = `wx-config --libs std,awx` -L${YBOO}/lib -lboost_system -lboost_thread -lwsock32 -lws2_32
#LDFLAGS += -static-libgcc -static-libstdc++ -s

### Targets: ###
Release: $(PROG)
cleanRelease:
	rm  *.o

### Link: ###
$(PROG): $(OBJECTS)
	@[ -d ./bin/Release ] || mkdir -p ./bin/Release
	g++ -o $(PROG) *.o $(LDFLAGS)


.cpp.o:
     $(CXX) $(CXXFLAGS) -o $@ $(CPPDEPS) $<


clean: cleanRelease
# Dependencies tracking:
-include ./*.d

