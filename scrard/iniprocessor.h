#pragma once
#include <wx/hashmap.h>
#include <vector>
/* Custom multi-line *.ini file parser with wx.button markup
Examples:

# single-line entry
signal=mixed

# multi-line entry
[sql1]
select *
from dual;

# single buttons
wx.button.Label1=http://home.com
wx.button.Label2=http://newhome.com

# button group example
[wx.button.myteam]
mylabel1=exec hostname
mylabel2=exec ssh
*/

class IniProcessor
{
public:

    wxStringToStringHashMap m_map_ini;  // ini file entries
    wxStringToStringHashMap m_map_btn;  // button action definition (exec, http ...)

    std::vector<std::pair<wxString,wxString>> m_vbuttons; // button name, group name
    std::vector<std::pair<wxString,wxString>> m_vgbuttons; // group button name, referenced button group

    IniProcessor();
    void ProcessFile( const wxString& pfile = "/ysw/wx312d/wxfolder/scrard/bin/Release/default.ini" );
    void SimpleParse( const wxString& line );
    bool VectorContainsValue( const std::vector<std::pair<wxString,wxString>>& vstringstring, const wxString& vval );
    void SetMap( const wxString& mkey, const wxString& mval, bool fGroupEntry = true );

};

