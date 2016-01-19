//////////////////////////////////////////////////////////////////////////////////////////////////
// Name:        wx-config adapted for use with mingw under PortableGit environment
// Purpose:     https://wiki.wxwidgets.org/Wx-Config
// Author:      YACoin Developers
// Created:     2016-02-29
// Licence:     wxWidgets licence
//////////////////////////////////////////////////////////////////////////////////////////////////
#include <wx/utils.h>
#include <wx/cmdline.h>

//#define YWX C:/PortableGit/base/deps/wxWidgets-3.1.0
//#include "wx-config.h"

static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
    { wxCMD_LINE_SWITCH, "h", "help" , "Please use only long switches", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_SWITCH, "r", "release" , "r 4 short and release for long switch" },        	// 3.0
    { wxCMD_LINE_SWITCH, "p", "prefix" , "p 4 short and prefix for long switch" },              // C:/PortableGit/base/deps/wxWidgets-3.0.1
	{ wxCMD_LINE_SWITCH, "e", "exec-prefix" , "e 4 short and exec-prefix for long switch" },    // C:/PortableGit/base/deps/wxWidgets-3.0.1
    { wxCMD_LINE_SWITCH, "b", "basename" , "b 4 short and basename for long switch" },          // wx_mswu
    { wxCMD_LINE_SWITCH, "v", "version" , "v 4 short and version for long switch" },            // 3.0.1
    { wxCMD_LINE_SWITCH, "g", "cc" , "g 4 short and cc for long switch" },                      // gcc
    { wxCMD_LINE_SWITCH, "x", "cxx" , "x 4 short and cxx for long switch" },                    // g++
    { wxCMD_LINE_SWITCH, "d", "debug" , "d 4 short and debug for long switch" },
    { wxCMD_LINE_SWITCH, "l", "libs" , "l 4 short and libs for long switch" },
    { wxCMD_LINE_SWITCH, "f", "cxxflags" , "f 4 short and cxxflags for long switch" },
	{ wxCMD_LINE_SWITCH, "a", "cflags" , "a 4 short and cflags for long switch" },
    { wxCMD_LINE_SWITCH, "s", "static" , "s 4 short and static for long switch" },
    { wxCMD_LINE_SWITCH, "o", "cppflags" , "o 4 short and cppflags for long switch" },
    { wxCMD_LINE_PARAM, NULL, NULL, "params", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL|wxCMD_LINE_PARAM_MULTIPLE  },
    { wxCMD_LINE_NONE }
};

int main(int argc, char **argv)
{
    wxString ywx = wxSTRINGIZE(YWX); if ( ywx.IsSameAs("YWX") ) ywx = wxEmptyString;
	
    wxString data = wxEmptyString;
    wxCmdLineParser parser( g_cmdLineDesc );
    parser.SetCmdLine( argc, argv );

	wxString wxver = wxEmptyString; // set here to ovverride, for example "3.0.1"
    wxUniChar ch = '-'; // for extracting release and version from wx path
    int pos = ywx.Find(ch, true);	// search backwards
    if ( wxver.IsEmpty() && pos != wxNOT_FOUND )
        wxver = ywx.Mid(++pos,5); 

	// wx flags
    bool wx(true); // include wx stuff
	bool sta(true); // wx standard libs
	bool bas(false); // wx base libs
	bool web(false); // wxWebView
	bool stc(false); // wxScintilla
	bool rch(false); // wxRichTextCtrl
	bool rib(false); // wxRibbon
	bool ppg(false); // wxPropertyGrid
	bool aui(false); // wxAUI
	bool ogl(false); // OpenGL

	bool jsn(false); // wxJSON
	bool plo(false); // wxPlotCtrl
	bool tgs(false); // wxThings
	bool fnb(false); // wxFlatNotebook
	bool awx(false); // libawx

	bool dbg(false);

    // need to set before calling parser.parse when using console only
    wxMessageOutputStderr out;
    wxMessageOutput::Set( &out );

    if ( parser.Parse(true) == 0 )
    {
        if ( argc > 1 )
        {
            if ( parser.Found("d") ) dbg = true;

            for ( int i = 0; i < parser.GetParamCount(); i++ )
            {
                wxArrayString parr = wxSplit( parser.GetParam(i), ',' );
                for ( int j = 0; j < parr.size(); j++ )
                {
                    if ( parr[j].IsSameAs("nowx") || ywx.IsEmpty() ) wx = false;
					else if ( wx && parr[j].IsSameAs("std") ) sta = true;
					else if ( wx && parr[j].IsSameAs("base") ) bas = true;
					else if ( wx && parr[j].IsSameAs("webview") ) web = true;
					else if ( wx && parr[j].IsSameAs("stc") ) stc = true;
					else if ( wx && parr[j].IsSameAs("richtext") ) rch = true;
					else if ( wx && parr[j].IsSameAs("ribbon") ) rib = true;
					else if ( wx && parr[j].IsSameAs("propgrid") ) ppg = true;
					else if ( wx && parr[j].IsSameAs("aui") ) aui = true;
					else if ( wx && parr[j].IsSameAs("gl") ) ogl = true;

					else if ( wx && parr[j].IsSameAs("plotctrl") ) plo = true;
					else if ( wx && parr[j].IsSameAs("things") ) tgs = true;
					else if ( wx && parr[j].IsSameAs("flatnotebook") ) fnb = true;
					else if ( wx && parr[j].IsSameAs("json") ) jsn = true;
					else if ( wx && parr[j].IsSameAs("awx") ) awx = true;
                    else
                        wxPuts(parr[j]);
                }
            }
			if ( plo ) tgs = true; // wxPlotCtrl needs wxThings
			if ( bas ) sta = false; // --libs base disables other libraries and -mwindows flag. Use for console apps.

            if ( parser.Found("g") ) data.Append( "gcc" ); // --cc
            if ( parser.Found("x") ) data.Append( dbg ? "g++ -std=gnu++0x -g -Og -Wno-deprecated-declarations " 
													  : "g++ -std=gnu++0x -O2 -Wno-deprecated-declarations " ); // --cxx
													  
			if ( parser.Found("v") ) data.Append( wxver ); // --version 3.0.1
            if ( parser.Found("r") ) data.Append( wxver.Mid(0,3)); // --release 3.0
            
            if ( parser.Found("b") ) data.Append( dbg ? "wx_mswud" : "wx_mswu" ); // --basename
            if ( parser.Found("p") || parser.Found("e") ) data.Append( ywx ); // --prefix --exec-prefix

			if ( parser.Found("a") ) // --cflags
				data.Append(" ");

            // COMPILE flags
            if ( parser.Found("f") || parser.Found("o") ) // --cxxflags --cppflags
            {
                if (wx) if (dbg) data.Append("-I").Append(ywx).Append("/lib/gcc_lib/mswud -I").Append(ywx).Append("/include -DHAVE_W32API_H -D__WXMSW__ -DDEBUG -D_UNICODE ");
						else data.Append("-I").Append(ywx).Append("/lib/gcc_lib/mswu -I").Append(ywx).Append("/include -DHAVE_W32API_H -D__WXMSW__ -DNDEBUG -D_UNICODE ");
            }

            // LINK options
            if ( parser.Found("l") ) // --libs
            {
                if ( parser.Found("s") ) data.Append("-static -static-libgcc -static-libstdc++ ").Append( dbg ? "" : "-s "); // --static

				if ( wx && !bas ) data.Append("-mwindows ");

                if (wx) data.Append("-L").Append(ywx).Append("/lib/gcc_lib ");
				if ( wx && jsn ) data.Append( dbg ? "-lwxjsond " : "-lwxjson ");
				if ( wx && plo ) data.Append( dbg ? "-lplotctrld " : "-lplotctrl ");
				if ( wx && fnb ) data.Append( dbg ? "-lwxflatnotebookd " : "-lwxflatnotebook ");
				if ( wx && awx ) data.Append( dbg ? "-lawxd " : "-lawx ");

				wxString libsNN = wxEmptyString;
				if ( wx && bas && !sta ) libsNN.Append( dbg ? "-lwxbaseNNud " : "-lwxbaseNNu "); // use base for console only apps
				if ( wx && sta ) libsNN.Append( dbg ? "-lwxmswNNud_xrc " : "-lwxmswNNu_xrc ");
				if ( wx && web ) libsNN.Append( dbg ? "-lwxmswNNud_webview " : "-lwxmswNNu_webview ");
				if ( wx && stc ) libsNN.Append( dbg ? "-lwxmswNNud_stc " : "-lwxmswNNu_stc ");
				// if exists add -lwx_mswu_qa-3.1 lib here
				if ( wx && sta ) libsNN.Append( dbg ? "-lwxbaseNNud_net -lwxmswNNud_html -lwxmswNNud_adv -lwxmswNNud_core -lwxbaseNNud_xml -lwxbaseNNud "
												   :"-lwxbaseNNu_net -lwxmswNNu_html -lwxmswNNu_adv -lwxmswNNu_core -lwxbaseNNu_xml -lwxbaseNNu " );
				if ( wx && rch ) libsNN.Append( dbg ? "-lwxmswNNud_richtext " : "-lwxmswNNu_richtext ");
				if ( wx && rib ) libsNN.Append( dbg ? "-lwxmswNNud_ribbon " : "-lwxmswNNu_ribbon ");
				if ( wx && ppg ) libsNN.Append( dbg ? "-lwxmswNNud_propgrid " : "-lwxmswNNu_propgrid ");
				if ( wx && aui ) libsNN.Append( dbg ? "-lwxmswNNud_aui " : "-lwxmswNNu_aui ");
				if ( wx && ogl ) libsNN.Append( dbg ? "-lwxmswNNud_gl " : "-lwxmswNNu_gl ");

				wxver.Replace(".","");
				size_t rplcnt = libsNN.Replace("NN",wxver.Mid(0,2),true);				
				data.Append(libsNN); 

				if ( wx && stc ) data.Append( dbg ? "-lwxscintillad " : "-lwxscintilla ");
				if ( wx && ogl ) data.Append( "-lopengl32 -lglu32 ");

				if ( wx && sta ) data.Append( dbg ? "-lwxregexud -lwxexpatd -lwxtiffd -lwxjpegd -lwxpngd -lwxzlibd " : "-lwxregexu -lwxexpat -lwxtiff -lwxjpeg -lwxpng -lwxzlib " );
				if ( wx && ( tgs || plo ) ) data.Append( dbg ? "-lwxthingsd " : "-lwxthings ");
                if (wx) data.Append("-lrpcrt4 -loleaut32 -lole32 -luuid -lwinspool -lwinmm -lshell32 -lcomctl32 -lcomdlg32 -ladvapi32 -lwsock32 -lgdi32 -lversion -lshlwapi");
            }
        }
    }
    else
    {
        wxPuts("Invalid program arguments!");
        return 1;
    }

    wxPuts(data);
    //system("pause");
    return 0;
}

