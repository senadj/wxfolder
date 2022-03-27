#include "iniprocessor.h"
#include <wx/textfile.h>

static bool fgroup_read; // flag denotes multi line reading under ini group section
static wxString group_key;
static wxString group_val;

IniProcessor::IniProcessor(){}

void IniProcessor::ProcessFile( const wxString& pfile )
{
    fgroup_read = false;
    group_key = "";
    group_val = "";

    wxTextFile tfile;
    tfile.Open(pfile, wxConvUTF8);

    for ( wxString fline = tfile.GetFirstLine(); !tfile.Eof(); fline = tfile.GetNextLine() )
        SimpleParse(fline);

    SimpleParse(""); // compensate missing empty line in ini file
    tfile.Close ();
}

void IniProcessor::SimpleParse( const wxString& line )
{
    if ( ! line.StartsWith("#") )
    {
        if ( line.StartsWith("[") )
        {
            if ( fgroup_read && ! group_val.IsEmpty() ) // close previous section, if open; happens when no empty line between sections
                SetMap(group_key,group_val);

            group_val = "";
            group_key = line.Mid(1, line.length()-2);
            fgroup_read = line.StartsWith("[=]")?false:true; // [=] group denotes switchover from multiline section back to parsable entries
        }
        else if ( ! line.IsEmpty() )
        {
            if ( fgroup_read )
            {
                if ( group_val.IsEmpty() ) { group_val = line;  }
                else { group_val = group_val + '\n' + line; }
            }
            else
            {
                int iend = line.Find("=");
                if ( iend != wxNOT_FOUND )
                {
                    wxString inikey = line.Mid(0,iend).Trim().Trim(false);
                    wxString inival = line.Mid(iend+1).Trim().Trim(false);
                    SetMap(inikey,inival,false /* fGroupEntry */);
                }
            }
        }
        else if ( fgroup_read && line.IsEmpty() )// first empty line ends multiline section
        {
            if ( ! group_val.IsEmpty() )
                SetMap(group_key,group_val);
            fgroup_read = false;
        }
    }
}

bool IniProcessor::VectorContainsValue( const std::vector<std::pair<wxString,wxString>>& vstringstring, const wxString& vval )
{
    for ( auto& it: vstringstring )
        if (it.second.IsSameAs(vval))
            return true;

    return false;
}

void IniProcessor::SetMap( const wxString& mkey, const wxString& mval, bool fGroupEntry )
{
    if ( mkey.StartsWith("wx.button.") )
    {
        wxString ref_button_group = wxEmptyString;
        wxString bgrp = mkey.Mid(10);

        if ( mval.IsSameAs(".") )   // wx.button.se11=.
            ref_button_group = bgrp;
        else if ( mval.StartsWith("[wx.button.") )  // wx.button.se11=[wx.button.abc]
            ref_button_group = mval.Mid(11,mval.length()-12);

        if ( ! ref_button_group.IsEmpty() )
            m_vgbuttons.push_back(std::make_pair(bgrp,ref_button_group));
        else
        {
            if ( fGroupEntry )
            {
                if ( ! VectorContainsValue(m_vgbuttons,bgrp) )
                    m_vgbuttons.push_back(std::make_pair(bgrp,bgrp));

                for (auto& bline: wxSplit(mval,'\n'))
                {
                    int iend = bline.Find("=");
                    if ( iend != wxNOT_FOUND )
                    {
                        wxString inikey = bline.Mid(0,iend).Trim().Trim(false);
                        wxString inival = bline.Mid(iend+1).Trim().Trim(false);
                        m_vbuttons.push_back(std::make_pair(inikey, bgrp));
                        m_map_btn[bgrp+"."+inikey] = inival; // button action map: curl, http...
                    }
                }
            }
            else    // single line button entry
            {
                if ( ! VectorContainsValue(m_vgbuttons,"unassigned") )
                    m_vgbuttons.push_back(std::make_pair("unassigned","unassigned"));

                m_vbuttons.push_back(std::make_pair(bgrp, "unassigned"));
                m_map_btn["unassigned."+bgrp] = mval; // button action: curl, http...
            }
        }
    }
    else
        m_map_ini[mkey] = mval;
}

