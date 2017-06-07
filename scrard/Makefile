PROG = ./bin/Release/$(notdir $(CURDIR)).exe
CPPDEPS = -MT$@ -MF`echo $@ | sed -e 's,\.o$$,.d,'` -MD -MP
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
#CXXFLAGS = `wx-config --cxxflags` `y-config --cxxflags boost` -std=gnu++0x -O0
#CXXFLAGS = `wx-config --cxxflags` `y-config --cxxflags boost` -std=gnu++0x -O0 -Wno-deprecated-declarations
CXXFLAGS = `wx-config --cxxflags` `y-config --cxxflags boost` -std=gnu++0x -O0 -D_WIN32_WINNT=0x0501 -Wno-deprecated-declarations
LDFLAGS = `wx-config --libs std,aws` -LD:/PortableGit/ydir/deps/boost_1_58_0/stage/lib -lboost_date_time-mgw51-mt-1_58 -lboost_system-mgw51-mt-1_58 -lboost_thread-mgw51-mt-1_58 -lwsock32 -lws2_32
#DLL: LDFLAGS = `wx-config --libs base,json` -static-libgcc -static-libstdc++ -s

### Targets: ###
Release: $(PROG)
cleanRelease:
	rm  xFrame.o xPanel.o xApp.o wxBoostSerial.o xComPort.o xScratchClient.o AsyncSerial.o
	#rm   xPanel.o xFrame.o #xApp.o xScratchClient.o
	#rm xScratchClient.o
	#rm   lineread.o
	#rm -f ./*.d; rm -f ./*.o; rm -f $(PROG)

### Link: ###
$(PROG): xApp.o	xPanel.o xFrame.o xComPort.o wxBoostSerial.o xScratchClient.o AsyncSerial.o
	@[ -d ./bin/Release ] || mkdir -p ./bin/Release
	g++ -o bin/Release/testx.exe AsyncSerial.o xApp.o xComPort.o xFrame.o xPanel.o wxBoostSerial.o xScratchClient.o `wx-config --libs std,awx` -LD:/PortableGit/ydir/deps/boost_1_58_0/stage/lib -lboost_regex-mgw51-mt-1_58 -lboost_date_time-mgw51-mt-1_58 -lboost_system-mgw51-mt-1_58 -lboost_thread-mgw51-mt-1_58 -lwsock32 -lws2_32



AsyncSerial.o:
	g++ `y-config --cxxflags boost` -std=gnu++0x -D_WIN32_WINNT=0x0501 -Wno-deprecated-declarations -I. -c AsyncSerial.cpp

wxBoostSerial.o:
	g++ `wx-config --cxxflags` `y-config --cxxflags boost` -std=gnu++0x -D_WIN32_WINNT=0x0501 -Wno-deprecated-declarations -I. -c wxBoostSerial.cpp

xComPort.o:
	g++ `wx-config --cxxflags` `y-config --cxxflags boost` -std=gnu++0x -D_WIN32_WINNT=0x0501 -Wno-deprecated-declarations -I. -c xComPort.cpp

xScratchClient.o:
	g++ `wx-config --cxxflags` `y-config --cxxflags boost` -std=gnu++0x -D_WIN32_WINNT=0x0501 -Wno-deprecated-declarations -I. -c xScratchClient.cpp

xApp.o:
	g++ `wx-config --cxxflags` `y-config --cxxflags boost` -std=gnu++0x -D_WIN32_WINNT=0x0501 -Wno-deprecated-declarations -I. -c xApp.cpp

xFrame.o:
	g++ `wx-config --cxxflags` `y-config --cxxflags boost` -std=gnu++0x -D_WIN32_WINNT=0x0501 -Wno-deprecated-declarations -I. -c xFrame.cpp

xPanel.o:
	g++ `wx-config --cxxflags` `y-config --cxxflags boost` -std=gnu++0x -D_WIN32_WINNT=0x0501 -Wno-deprecated-declarations -I. -c xPanel.cpp

main.o:
	g++ `wx-config --cxxflags` `y-config --cxxflags boost` -std=gnu++0x -D_WIN32_WINNT=0x0501 -Wno-deprecated-declarations -I. -c main.cpp

clean: cleanRelease
# Dependencies tracking:
-include ./*.d

