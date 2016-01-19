PROG = ./bin/Release/$(notdir $(CURDIR)).exe
CPPDEPS = -MT$@ -MF`echo $@ | sed -e 's,\.o$$,.d,'` -MD -MP
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard src/*.cpp))
CXX = `wx-config --cxx`
CXXFLAGS = `wx-config --cxxflags` -DWXMAKINGDLL_BITZ -DPIC
#CXXFLAGS = `wx-config --cxxflags`
LDFLAGS = `wx-config --libs core`


### Targets: ###
Release: $(PROG)
cleanRelease:
	rm -f ./*.d; rm -f ./*.o; rm -f ./src/*.o; rm -f ./src/*.d; rm -f ./*.a; rm -f ./*.dll; rm -f $(PROG)
clean: cleanRelease

### Link: ###
$(PROG): bitz.dll main.o
	@[ -d ./bin/Release ] || mkdir -p ./bin/Release
	$(CXX) -o $@ main.o $(LDFLAGS) -L. -lbitz


### Compile: ###
.cpp.o:
	$(CXX) $(CXXFLAGS) -I./include/wx -c -o $@ $(CPPDEPS) $<

main.o:
	`wx-config --cxx` `wx-config --cxxflags` -I/ydir/deps/boost_1_58_0 -I./include -c -o main.o -MTmain.o -MFmain.d -MD -MP main.cpp

bitz.dll: $(OBJECTS)
	g++ -shared ./src/bitz.o -o bitz.dll `wx-config --libs core` -Wl,--out-implib,libbitz.dll.a


# Dependencies tracking:
-include ./*.d ./src/*.d
