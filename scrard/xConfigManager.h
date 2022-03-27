#pragma once
#include <wx/fileconf.h>

class xConfigManager
{
    public:
        wxFileConfig* m_cfg;
        xConfigManager( const wxString& inifile );
        ~xConfigManager();
        bool Exists( const wxString& strName ); // suitable for groups or entries
        bool HasEntry( const wxString& strName );
        bool HasGroup( const wxString& strName );
        const wxString ReadEntry ( const wxString& key, const wxString& defaultVal );
        size_t GetGroups( wxString path, wxArrayString& groups );
        size_t GetEntries( wxString path, wxVector<std::pair<wxString,wxString>>& entries );
};
