//////////////////////////////////////////////////////////////////////////////////////////////////
// Name:        y-config
// Purpose:     Support for OpenSSL, Boost, QRencode, BerkeleyDB, miniUPNPc, PCRE, Jansson, cURL
// Author:      YACoin Developers
// Created:     2016-02-29
// Licence:     wxWidgets licence
//////////////////////////////////////////////////////////////////////////////////////////////////
#include <wx/utils.h>
#include <wx/cmdline.h>
#include "y-config.h"

static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
    { wxCMD_LINE_SWITCH, "h", "help" , "Please use only long switches", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_SWITCH, "v", "version" , "v 4 short and version for long switch" },            // 0.1.0
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
    wxString yossl = wxSTRINGIZE(YOSSL); if ( yossl.IsSameAs("YOSSL") ) yossl = wxEmptyString;
    wxString ybdb = wxSTRINGIZE(YBDB); if ( ybdb.IsSameAs("YBDB") ) ybdb = wxEmptyString;
	wxString yldb = wxSTRINGIZE(YLDB); if ( yldb.IsSameAs("YLDB") ) yldb = wxEmptyString;
    wxString yupnp = wxSTRINGIZE(YUPNP); if ( yupnp.IsSameAs("YUPNP") ) yupnp = wxEmptyString;
	wxString yzlib = wxSTRINGIZE(YZLIB); if ( yzlib.IsSameAs("YZLIB") ) yzlib = wxEmptyString;
    wxString yqr = wxSTRINGIZE(YQR); if ( yqr.IsSameAs("YQR") ) yqr = wxEmptyString;
	wxString ypcre = wxSTRINGIZE(YPCRE); if ( ypcre.IsSameAs("YPCRE") ) ypcre = wxEmptyString;
	wxString yjans = wxSTRINGIZE(YJANS); if ( yjans.IsSameAs("YJANS") ) yjans = wxEmptyString;
	wxString ycurl = wxSTRINGIZE(YCURL); if ( ycurl.IsSameAs("YCURL") ) ycurl = wxEmptyString;
	wxString yboo = wxSTRINGIZE(YBOO); if ( yboo.IsSameAs("YBOO") ) yboo = wxEmptyString;
    wxString yboostlibs = wxSTRINGIZE(YBOOSTLIBS); if ( yboostlibs.IsSameAs("YBOOSTLIBS") ) yboostlibs = wxEmptyString;
	wxString yboostlibsd = wxSTRINGIZE(YBOOSTLIBSD); if ( yboostlibsd.IsSameAs("YBOOSTLIBSD") ) yboostlibsd = wxEmptyString;

    wxString data = wxEmptyString;
    wxCmdLineParser parser( g_cmdLineDesc );
    parser.SetCmdLine( argc, argv );

	bool dbg(false);

	// supported libs
    bool ssl(false);
    bool boo(false);
    bool bdb(false);
    bool pnp(false);
	bool zli(false);
    bool qre(false);
	bool pcr(false);
	bool jan(false);
	bool cur(false);
	bool ldb(false);

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
                    if ( parr[j].IsSameAs("openssl") && !yossl.IsEmpty() ) ssl = true;
                    else if ( parr[j].IsSameAs("boost") && !yboo.IsEmpty() ) boo = true;
                    else if ( parr[j].IsSameAs("berkeleydb") && !ybdb.IsEmpty() ) bdb = true;
					else if ( parr[j].IsSameAs("leveldb") && !yldb.IsEmpty() ) ldb = true;
                    else if ( parr[j].IsSameAs("upnp") && !yupnp.IsEmpty() ) pnp = true;
					else if ( parr[j].IsSameAs("zlib") && !yzlib.IsEmpty() ) zli = true;
                    else if ( parr[j].IsSameAs("qrencode") && !yqr.IsEmpty() ) qre = true;
					else if ( parr[j].IsSameAs("pcre") && !ypcre.IsEmpty() ) pcr = true;
					else if ( parr[j].IsSameAs("jansson") && !yjans.IsEmpty() ) jan = true;
					else if ( parr[j].IsSameAs("curl") && !ycurl.IsEmpty() ) cur = true;
                    else
                        wxPuts(parr[j]);
                }
            }

            if ( parser.Found("g") ) data.Append( "gcc" ); // --cc
            if ( parser.Found("x") ) data.Append( dbg ? "g++ -std=gnu++0x -g -Og -Wno-deprecated-declarations " 
													  : "g++ -std=gnu++0x -O2 -Wno-deprecated-declarations " ); // --cxx
													  
			if ( parser.Found("v") ) data.Append( "0.1.0" ); // --version 1.0.1

			if ( parser.Found("a") ) // --cflags
				data.Append(" ");

            // COMPILE flags
            if ( parser.Found("f") || parser.Found("o") ) // --cxxflags --cppflags
            {
                if (boo) data.Append("-I").Append(yboo).Append(" ");
				if (cur) data.Append("-DCURL_STATICLIB -I").Append(ycurl).Append("/include ");	// should come before openssl
                if (ssl) data.Append("-I").Append(yossl).Append("/build/include ");
                if (bdb) data.Append("-I").Append(ybdb).Append("/build_unix ");
				if (ldb) data.Append("-I").Append(yldb).Append("/include ").Append("-I").Append(yldb).Append("/helpers/memenv ");
                if (pnp) data.Append("-I").Append(yupnp).Append(" ");
				if (zli) data.Append("-I").Append(yzlib).Append(" ");
                if (qre) data.Append("-I").Append(yqr).Append(" ");
				if (pcr) data.Append("-DPCRE_STATIC -I").Append(ypcre).Append(" ");
				if (jan) data.Append("-I").Append(yjans).Append("/src ");				
            }

            // LINK options
            if ( parser.Found("l") ) // --libs
            {
                if ( parser.Found("s") ) data.Append("-static -static-libgcc -static-libstdc++ ").Append( dbg ? "" : "-s "); // --static

                if (boo) data.Append("-L").Append(yboo).Append("/stage/lib ").Append( dbg ? yboostlibsd : yboostlibs ).Append(" ");
				if (cur) data.Append("-L").Append(ycurl).Append("/lib/.libs -lcurl ");
                if (ssl) data.Append("-L").Append(yossl).Append("/build/lib -lcrypto -lssl ");
                if (bdb) data.Append("-L").Append(ybdb).Append("/build_unix -ldb-4.8 -ldb -ldb_cxx-4.8 -ldb_cxx ");
				if (ldb) data.Append("-L").Append(yldb).Append(" -lmemenv -lleveldb ");
                if (pnp) data.Append("-L").Append(yupnp).Append(" -lminiupnpc ");
				if (zli) data.Append("-L").Append(yzlib).Append(" -lz ");
                if (qre) data.Append("-L").Append(yqr).Append("/.libs -lqrencode ");
				if (pcr) data.Append("-L").Append(ypcre).Append("/.libs -lpcre ");
				if (jan) data.Append("-L").Append(yjans).Append("/src/.libs -ljansson ");
				
				if (cur) data.Append("-lws2_32 -lwldap32 ");
            }
        }
		else
		{
			if ( !yossl.IsEmpty() ) data.Append("openssl\n");
			if ( !ybdb.IsEmpty() ) data.Append("berkeleydb\n");
			if ( !yldb.IsEmpty() ) data.Append("leveldb\n");
			if ( !yupnp.IsEmpty() ) data.Append("upnp\n");
			if ( !yzlib.IsEmpty() ) data.Append("zlib\n");
			if ( !yqr.IsEmpty() ) data.Append("qrencode\n");
			if ( !ypcre.IsEmpty() ) data.Append("pcre\n");
			if ( !yjans.IsEmpty() ) data.Append("jansson\n");
			if ( !ycurl.IsEmpty() ) data.Append("curl\n");
			if ( !yboo.IsEmpty() ) data.Append("boost\n");
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

