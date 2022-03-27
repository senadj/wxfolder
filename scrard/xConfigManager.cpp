#include "xConfigManager.h"
#include "xApp.h"

xConfigManager::~xConfigManager()
{
    wxConfigBase::Set( NULL );
    wxDELETE(m_cfg);
}

xConfigManager::xConfigManager( const wxString& inifile )
{
	m_cfg = new wxFileConfig("ScrArdCfg","senadj",inifile,wxEmptyString,wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
    wxConfigBase::Set( m_cfg );
}

bool xConfigManager::Exists( const wxString& strName ) { return m_cfg->Exists(strName); }
bool xConfigManager::HasEntry( const wxString& strName ) { return m_cfg->HasEntry(strName); }
bool xConfigManager::HasGroup( const wxString& strName ) { return m_cfg->HasGroup(strName); }
const wxString xConfigManager::ReadEntry ( const wxString& key, const wxString& defaultVal ) { return m_cfg->Read(key, defaultVal); }

size_t xConfigManager::GetGroups( wxString path, wxArrayString& groups )
{
    wxString str;
    long idx;
    bool hasData(false);

    groups.Empty();
    m_cfg->SetPath(path);
    size_t groupCnt = m_cfg->GetNumberOfGroups();

    if (groupCnt == 0)
        return 0;

    if (! m_cfg->GetFirstGroup(str, idx) )
        groups.Add(str);
    else
        hasData = true;


    while ( hasData )
    {
        groups.Add(str);
        hasData = m_cfg->GetNextGroup(str, idx);
    }

    return groupCnt;
}

size_t xConfigManager::GetEntries( wxString path, wxVector<std::pair<wxString,wxString>>& entries )
{
    wxString str;
    long idx;
    bool hasData(false);

    entries.clear();
    m_cfg->SetPath(path);
    size_t keyCnt = m_cfg->GetNumberOfEntries();

    if (keyCnt == 0)
        return 0;

    if (! m_cfg->GetFirstEntry(str, idx) )
    {
        entries.push_back(std::make_pair( str, m_cfg->Read(m_cfg->GetPath() + "/" + str) ));
    }
    else
        hasData = true;


    while ( hasData )
    {
        entries.push_back(std::make_pair( str, m_cfg->Read(m_cfg->GetPath() + "/" + str) ));
        hasData = m_cfg->GetNextEntry(str, idx);
    }

    return keyCnt;
}


