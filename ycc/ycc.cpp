// g++ `wx-config --cxxflags base` main.cpp -c && g++ main.o `wx-config --libs base` -s
#include <wx/wx.h>
#include <wx/regex.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/filename.h>
#include <wx/dir.h>

// careful "/ysw/" get's replaced below with YSW define
// define YCC4WXDLL if target wx libs are dlls
#ifdef __WINDOWS__
#define YSW "C:\\"
#if defined(YCC4WXDLL)
#define CXXFLAGSBASE "-I/ysw/wx328d -D_LARGEFILE_SOURCE=unknown -DwxDEBUG_LEVEL=0 -DWXUSINGDLL -D__WXMSW__ -DwxUSE_GUI=0 -mthreads"
#define CXXFLAGS "-I/ysw/wx328d -D_LARGEFILE_SOURCE=unknown -DwxDEBUG_LEVEL=0 -DWXUSINGDLL -D__WXMSW__ -mthreads"
#define LIBSBASE "-L/ysw/wx328d ###XXX### ###XMLNET### -lwx_baseu-3.2"
#define LIBSCORE "-L/ysw/wx328d -Wl,--subsystem,windows -mwindows ###XXX### -lwx_mswu_core-3.2 ###XMLNET### -lwx_baseu-3.2"
#define LIBSSTD "-L/ysw/wx328d -Wl,--subsystem,windows -mwindows ###XXX### -lwx_mswu_xrc-3.2 -lwx_mswu_html-3.2 -lwx_mswu_qa-3.2 -lwx_mswu_core-3.2 -lwx_baseu_xml-3.2 -lwx_baseu_net-3.2 -lwx_baseu-3.2"
#else	// end YCC4WXDLL. Target static wx lib:
#define CXXFLAGSBASE "-I/ysw/wx328 -D_FILE_OFFSET_BITS=64 -DwxDEBUG_LEVEL=0 -D__WXMSW__ -DwxUSE_GUI=0 -mthreads"
#define CXXFLAGS "-I/ysw/wx328 -D_FILE_OFFSET_BITS=64 -DwxDEBUG_LEVEL=0 -D__WXMSW__ -mthreads"
#define LIBSBASE "-L/ysw/wx328 ###XXX### ###XMLNET### -lwx_baseu-3.2 -lwxtiff-3.2 -lwxjpeg-3.2 -lwxpng-3.2 -lwxregexu-3.2 -lwxscintilla-3.2 -lwxexpat-3.2 -lwxzlib-3.2 -lrpcrt4 -loleaut32 -lole32 -luuid -luxtheme -lwinspool -lwinmm -lshell32 -lshlwapi -lcomctl32 -lcomdlg32 -ladvapi32 -lversion -lws2_32 -lgdi32 -loleacc -lwinhttp"
#define LIBSCORE "-L/ysw/wx328 -Wl,--subsystem,windows -mwindows ###XXX### -lwx_mswu_core-3.2 ###XMLNET### -lwx_baseu-3.2 -lwxtiff-3.2 -lwxjpeg-3.2 -lwxpng-3.2 -lwxregexu-3.2 -lwxscintilla-3.2 -lwxexpat-3.2 -lwxzlib-3.2 -lrpcrt4 -loleaut32 -lole32 -luuid -luxtheme -lwinspool -lwinmm -lshell32 -lshlwapi -lcomctl32 -lcomdlg32 -ladvapi32 -lversion -lws2_32 -lgdi32 -loleacc -lwinhttp"
#define LIBSSTD  "-L/ysw/wx328 -Wl,--subsystem,windows -mwindows ###XXX### -lwx_mswu_xrc-3.2 -lwx_mswu_html-3.2 -lwx_mswu_qa-3.2 -lwx_mswu_core-3.2 -lwx_baseu_xml-3.2 -lwx_baseu_net-3.2 -lwx_baseu-3.2 -lwxtiff-3.2 -lwxjpeg-3.2 -lwxpng-3.2 -lwxregexu-3.2 -lwxscintilla-3.2 -lwxexpat-3.2 -lwxzlib-3.2 -lrpcrt4 -loleaut32 -lole32 -luuid -luxtheme -lwinspool -lwinmm -lshell32 -lshlwapi -lcomctl32 -lcomdlg32 -ladvapi32 -lversion -lws2_32 -lgdi32 -loleacc -lwinhttp"
#endif	// end Windows
#else	// Linux
#define YSW "/"
#define CXXFLAGSBASE "-I/ysw/wx331d -D_FILE_OFFSET_BITS=64 -DwxDEBUG_LEVEL=0 -DWXUSINGDLL -D__WXGTK__ -DwxUSE_GUI=0 -pthread"
#define CXXFLAGS "-I/ysw/wx331d -D_FILE_OFFSET_BITS=64 -DwxDEBUG_LEVEL=0 -DWXUSINGDLL -D__WXGTK__ -pthread"
#define LIBSBASE "-L/ysw/wx331d -pthread -Wl,-rpath,/ysw/wx331d ###XXX### ###XMLNET### -lwx_baseu-3.3"
#define LIBSCORE "-L/ysw/wx331d -pthread -Wl,-rpath,/ysw/wx331d ###XXX### -lwx_gtk3u_core-3.3 ###XMLNET### -lwx_baseu-3.3"
#define LIBSSTD  "-L/ysw/wx331d -pthread -Wl,-rpath,/ysw/wx331d ###XXX### -lwx_gtk3u_xrc-3.3 -lwx_gtk3u_html-3.3 -lwx_gtk3u_qa-3.3 -lwx_gtk3u_core-3.3 -lwx_baseu_xml-3.3 -lwx_baseu_net-3.3 -lwx_baseu-3.3"
#endif

int ExecuteCommand(const wxString& cmd, const wxExecuteEnv& env)
{
	wxArrayString output;
	wxArrayString errors;
	
	wxPuts(cmd);
	wxExecute( cmd,output,errors,wxEXEC_SYNC,&env );
	if ( !errors.empty() )
	{
		for (auto& str: errors)
			wxFputs(str,stderr);
		return 0;
	}

	for (auto& str: output)
		wxPuts(str);

	return 1;
}

void ExtractBackquotedData(const wxString& input, wxArrayString& backquoted) 
{
    wxRegEx regex("`([^`]*)`");
    wxString remaining = input;

    while (regex.Matches(remaining)) 
	{
        wxString match = regex.GetMatch(remaining, 1);  // Group 1: content inside backquotes	//wxLogMessage("Found: %s", match);

        // Remove everything up to and including the current match
        wxString fullMatch = regex.GetMatch(remaining); // This includes the backquotes
		backquoted.Add(fullMatch);	//wxLogMessage("FoundFull: %s", fullMatch);
        int matchPos = remaining.Find(fullMatch);
        if (matchPos == wxNOT_FOUND) break;

        remaining = remaining.Mid(matchPos + fullMatch.length());
    }
}

bool SetBuildCommandFromSourceFile(const wxString& fname, wxString& fline)
{
	bool ferror = false;
	wxTextFile tfile;
	tfile.Open ( fname, wxConvUTF8 );
	if ( tfile.GetLineCount() < 1 )
	{
		wxFputs(wxString::Format("No Lines found in file:\n%s\n",fname),stderr);
		ferror = true;
	}
	else
	{
		fline = tfile.GetFirstLine().Trim().Trim(false);
		if ( ! fline.Mid(0,2).IsSameAs("//") )
		{
			wxFputs(wxString::Format("No comment found in first line:\n%s\n",fname),stderr);
			ferror = true;
		}
		fline = fline.Mid(2);	// remove "//"
	}

	tfile.Close();
	return ferror;
}


int main(int argc, char **argv)
{	
		wxInitializer initializer;     // optional in some cases

		size_t fcnt = 0;
		wxArrayString cppfiles;
		
		if (argc==1) // 1 argument = exe filename only (basically run without args)
			fcnt = wxDir::GetAllFiles(".",&cppfiles,"*.cpp",wxDIR_FILES);
		
		// check *.cpp file, extract path
		wxFileName file((argc>1)?wxString(argv[1]):((fcnt==1)?cppfiles[0]:"main.cpp"));
		
		if ( ! file.IsOk() ) {	wxFputs(wxString::Format("File NOT IsOk():\n%s\n",file.GetFullPath()),stderr); return -1; }
		if ( ! file.FileExists() ) { wxFputs(wxString::Format("File NOT Exists():\n%s\n",file.GetFullPath()),stderr); return -1; }
		if ( ! file.IsAbsolute() )
			file.Normalize(wxPATH_NORM_DOTS|wxPATH_NORM_ABSOLUTE|wxPATH_NORM_LONG);	// remove dots, create absolute path...

		wxString build_cmd = "g++ `wx-config --cxxflags base` main.cpp -c && g++ main.o `wx-config --libs base`";
		if ( SetBuildCommandFromSourceFile( file.GetFullPath(), build_cmd ))
			return -1;

		wxArrayString backquoted_arr;
		ExtractBackquotedData( build_cmd, backquoted_arr );

		// use source file to extract working directory for compiler and set env
		wxExecuteEnv env;
		wxGetEnvMap(&env.env); // Get environment from parent
		env.cwd = file.GetPath(); // wxExecute should first "cd" to directory containing source file

		// extract library names from LIBSSTD define
		wxString core_template_lib = "";
		wxString net_lib = "";
		wxString xml_lib = "";
		wxStringTokenizer tkstd(LIBSSTD," ");
		while ( tkstd.HasMoreTokens() )
		{
			wxString linklib = tkstd.GetNextToken();
			if (linklib.Find("core") != wxNOT_FOUND) core_template_lib = linklib;	// used for replacement (json,plotctrl...)
			if (linklib.Find("net") != wxNOT_FOUND) net_lib = linklib;
			if (linklib.Find("xml") != wxNOT_FOUND) xml_lib = linklib;
		}

		// make replacements to compiler commands ``
		for (int i=0; i < backquoted_arr.size(); i++)
		{
			wxString data = backquoted_arr[i];
			data.Replace("`","",true);
			data.Replace(" ","",true);
			
			if ( data.StartsWith("wx-config--libs") )
			{
				bool fbase = false;
				bool fcore = false;
				bool fstd = false;
				
				wxString abc_libs = "";	// base core std 
				wxString xxx_libs = ""; // json plotctrl sqlite3 ...
				wxString xmlnet_libs = ""; // xml net
				
				wxStringTokenizer libsTkz(data.Mid(15),",");
				
				while ( libsTkz.HasMoreTokens() )
				{
					wxString libtokn = libsTkz.GetNextToken();
					
					if ( libtokn.IsSameAs("base") && !(fcore||fstd) ) 
					{
						abc_libs = LIBSBASE;
						fbase = true;
					}
					else if ( libtokn.IsSameAs("core") && !fstd )
					{
						abc_libs = LIBSCORE;
						fcore = true;
					}
					else if ( libtokn.IsSameAs("std") )
					{
						abc_libs = LIBSSTD;
						fstd = true;
					}
					else if ( libtokn.IsSameAs("xml") ) xmlnet_libs.Append(xml_lib + " ");
					else if ( libtokn.IsSameAs("net") ) xmlnet_libs.Append(net_lib + " ");
					else 
					{
						wxString my_lib = core_template_lib;
						my_lib.Replace("core",libtokn);
						xxx_libs.Append( my_lib + " " );
					}
				}
				
				if ( abc_libs.IsEmpty() )
					if ( ! xxx_libs.IsEmpty() || ! xmlnet_libs.IsEmpty() )
						abc_libs = LIBSBASE;
					else
						abc_libs = LIBSSTD;
				
				abc_libs.Replace("###XMLNET### ", xmlnet_libs);
				abc_libs.Replace("###XXX### ", xxx_libs);
				build_cmd.Replace(backquoted_arr[i],abc_libs);
			}
			else if (data.IsSameAs("wx-config--cxx"))
				build_cmd.Replace(backquoted_arr[i],"g++");
			else if (data.IsSameAs("wx-config--cxxflagsbase"))
				build_cmd.Replace(backquoted_arr[i],CXXFLAGSBASE);
			else if (data.IsSameAs("wx-config--cxxflags"))
				build_cmd.Replace(backquoted_arr[i],CXXFLAGS);
		}
		
		build_cmd.Replace("/ysw/",YSW);

		// replace main.cpp and main.o if needed in first line build comment
		wxString ftmp = file.GetFullName();
		if ( ! ftmp.IsSameAs("main.cpp") && build_cmd.Find(" main.cpp") != wxNOT_FOUND )
		{
			wxPuts(wxString::Format("# ycc REPLACE IN BUILD COMMAND: main.cpp -> %s", ftmp));
			build_cmd.Replace(" main.cpp", " " + ftmp);
			ftmp.Replace(".cpp",".o");
			build_cmd.Replace(" main.o", " " + ftmp);
		}
		
		wxStringTokenizer cmdsTkz(build_cmd,"&&");		
		while ( cmdsTkz.HasMoreTokens() )
		{
			wxString cmdtokn = cmdsTkz.GetNextToken().Trim().Trim(false);
			
			if ( ! cmdtokn.IsEmpty()) 
				if ( ! ExecuteCommand(cmdtokn,env) )
					return -1;
		}
		
		return 0;
}

