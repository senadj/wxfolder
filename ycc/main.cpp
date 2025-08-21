// g++ `wx-config --cxxflags base` main.cpp -c && g++ main.o `wx-config --libs base` -s
#include <wx/wx.h>
#include <wx/regex.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/filename.h>

// careful "/ysw/" get's replaced below with YSW define
#define YSW "C:\\"
#define CXXFLAGSBASE "-I/ysw/wx328d -D_LARGEFILE_SOURCE=unknown -DwxDEBUG_LEVEL=0 -DWXUSINGDLL -D__WXMSW__ -DwxUSE_GUI=0 -mthreads"
#define CXXFLAGS "-I/ysw/wx328d -D_LARGEFILE_SOURCE=unknown -DwxDEBUG_LEVEL=0 -DWXUSINGDLL -D__WXMSW__ -mthreads"
#define LIBSBASE "-L/ysw/wx328d ###XXX### ###XMLNET### -lwx_baseu-3.2"
#define LIBSCORE "-L/ysw/wx328d -Wl,--subsystem,windows -mwindows ###XXX### -lwx_mswu_core-3.2 ###XMLNET### -lwx_baseu-3.2"
#define LIBSSTD "-L/ysw/wx328d -Wl,--subsystem,windows -mwindows ###XXX### -lwx_mswu_xrc-3.2 -lwx_mswu_html-3.2 -lwx_mswu_qa-3.2 -lwx_mswu_core-3.2 -lwx_baseu_xml-3.2 -lwx_baseu_net-3.2 -lwx_baseu-3.2"

wxArrayString output;
wxArrayString errors;

int ExecuteCommand(const wxString& cmd, const wxExecuteEnv& env)
{
	wxPuts(cmd);
	wxExecute( cmd,output,errors,wxEXEC_SYNC,&env );
	if ( !errors.empty() )
	{
		for (auto& str: errors)
			wxPuts(str);
		return 0;
	}

	for (auto& str: output)
		wxPuts(str);
	
	output.clear();
	errors.clear();
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


int main(int argc, char **argv)
{	
		wxInitializer initializer;     // optional in some cases
   		
		wxString myline = "g++ `wx-config --cxxflags base` main.cpp -c && g++ main.o `wx-config --libs base`";
		
		// check *.cpp file, extract path
		wxFileName file((argc>1)?argv[1]:"main.cpp");
		
		if ( ! file.IsOk() ) {	wxPrintf("File NOT IsOk():\n%s",argv[1]); return -1; }
		if ( ! file.FileExists() ) { wxPrintf("File NOT Exists():\n%s",argv[1]); return -1; }
		if ( ! file.IsAbsolute() )
			file.Normalize(wxPATH_NORM_DOTS|wxPATH_NORM_ABSOLUTE|wxPATH_NORM_LONG);	// remove dots, create absolute path...
		
		// use source file to extract working directory for compiler and set env
		wxExecuteEnv env;
		wxGetEnvMap(&env.env); // Get environment from parent
		env.cwd = file.GetPath(); // wxExecute should first "cd" to directory containing source file

		// extract first line from source file
		wxTextFile tfile;
		tfile.Open ( file.GetFullPath(), wxConvUTF8 );
		myline = tfile.GetFirstLine().Mid(2).Trim().Trim(false);
		tfile.Close();
		
		wxArrayString backquoted_arr;
		ExtractBackquotedData( myline, backquoted_arr );

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
				myline.Replace(backquoted_arr[i],abc_libs);
			}
			else if (data.IsSameAs("wx-config--cxx"))
				myline.Replace(backquoted_arr[i],"g++");
			else if (data.IsSameAs("wx-config--cxxflagsbase"))
				myline.Replace(backquoted_arr[i],CXXFLAGSBASE);
			else if (data.IsSameAs("wx-config--cxxflags"))
				myline.Replace(backquoted_arr[i],CXXFLAGS);
		}
		
		myline.Replace("/ysw/",YSW);
		
		wxStringTokenizer cmdsTkz(myline,"&&");		
		while ( cmdsTkz.HasMoreTokens() )
		{
			wxString cmdtokn = cmdsTkz.GetNextToken().Trim().Trim(false);
			
			if ( !cmdtokn.IsEmpty()) 
				if ( ! ExecuteCommand(cmdtokn,env) )
					return -1;
		}
		
		return 0;
}

