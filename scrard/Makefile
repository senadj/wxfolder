PROG = ./bin/Release/$(notdir $(CURDIR)).exe
CPPDEPS = -MT$@ -MF`echo $@ | sed -e 's,\.o$$,.d,'` -MD -MP
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
CXXFLAGS = `wx-config --cxxflags` -I${YBOO} -std=gnu++0x -D_WIN32_WINNT=0x0501 -Wno-deprecated-declarations -I. -c
LDFLAGS = `wx-config --libs std,awx` -L${YBOO}/stage/lib -lboost_system-mgw51-mt-1_58 -lboost_thread-mgw51-mt-1_58 -lwsock32 -lws2_32
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

