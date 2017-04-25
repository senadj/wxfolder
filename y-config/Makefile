all:
	g++ -std=gnu++0x `wx-config --cxxflags` y-config.cpp `wx-config --libs base,json` -static -static-libgcc -static-libstdc++ -s -o ${YWX}/bin/y-config.exe
	[ -f "${YWX}/bin/y-config.ini" ] || cp y-config.ini ${YWX}/bin/