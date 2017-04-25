#include <queue>
#include <deque>
#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/stdpaths.h>
#include <wx/fileconf.h>
#include <wx/tokenzr.h>
#include <wx/hashset.h>
#include <wx/hashmap.h>
#include <wx/jsonreader.h>

#define INI_CMD_IMPORT "IMPORT"
#define INI_CMD_UNION "UNION"
#define INI_CMD_APPEND "APPEND"
#define INI_CMD_JSON "JSON"

#define wxEOL wxString("\n" )

WX_DECLARE_HASH_SET( wxString, wxStringHash, wxStringEqual, StringHashSet );
typedef wxVector<std::pair<wxString,wxString>> kvtype;  // key-value type for *.ini file entries


class xFileConfig : public wxFileConfig
{
    public:

        wxArrayString m_global_appends; // global append data (from APPEND key)
        wxArrayString m_groups; // *ini file groups sorted in ProcessCmdLine
        StringHashSet m_opts_set;   // *ini file subgroups (=cmd line --options)
        kvtype m_kv_vec, m_kv_cascade_vec; // vectors of key value pairs, cascade in case or IMPORT, UNION
        kvtype m_kv_append_vec; // vector with append data (append command in JSON)
        kvtype m_final_list_vec; // vector with final filtered entries from sorted groups
        wxStringToStringHashMap m_kv_nvl_map, m_kv_map, m_kv_cmd_map; // key value maps
        std::queue<wxString> m_union_que, m_import_que;  // import que for usage with multiple import directives
        std::deque<wxStringToStringHashMap> m_nvl_que;
        bool m_nvl_transform;

        const wxCmdLineParser* m_parser;

        wxStringToStringHashMap::const_iterator cascade_kv_iter;

        xFileConfig(wxString& inifname)
            : wxFileConfig("y-config","YACoin Developers",inifname,wxEmptyString,wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_NO_ESCAPE_CHARACTERS)
        {
            wxConfigBase::Set( this );
        }

        bool ParseJSON(const wxString& jsonstring, wxJSONValue& root)
        {
            wxJSONReader reader;
            int numErrors = reader.Parse( jsonstring, &root );
            if ( numErrors > 0 )  {
                wxPuts("ERROR: the JSON document is not well-formed");
                const wxArrayString& errors = reader.GetErrors();
                for ( wxArrayString::const_iterator it = errors.begin(); it != errors.end(); ++it )
                    wxPuts((*it));

                return false;
            }
            return true;
        }

        size_t GetCfgGroupsArray( const wxString path, wxArrayString& groups )
        {
            wxString str;
            long idx;
            bool hasData(false);

            groups.Empty();
            this->SetPath(path);
            size_t groupCnt = this->GetNumberOfGroups();

            if (groupCnt == 0)
                return 0;

            if (! this->GetFirstGroup(str, idx) )
                groups.Add(str);
            else
                hasData = true;


            while ( hasData )
            {
                groups.Add(str);
                hasData = this->GetNextGroup(str, idx);
            }

            return groupCnt;
        }

        void AddToQueue(std::queue<wxString>& que, wxString& target_val)
        {
            if (!que.empty() && que.front().IsSameAs(target_val))
                return;

            if ( this->HasGroup("/" + target_val) )
            {
                que.push( target_val );
            }
        }

        void FilterEntries (kvtype& entry_vec, const wxString& entry_key )
        {
            if ( entry_key.IsSameAs(INI_CMD_IMPORT) )
            {
                AddToQueue( m_import_que, this->Read(this->GetPath() + "/" + entry_key).Trim() );
            }
            else if ( entry_key.IsSameAs(INI_CMD_UNION) )
            {
                AddToQueue( m_union_que, this->Read(this->GetPath() + "/" + entry_key).Trim() );
            }
            else if ( entry_key.IsSameAs(INI_CMD_APPEND) )
            {
                m_global_appends.Add( this->Read(this->GetPath() + "/" + entry_key).Trim() );
            }
            else if ( entry_key.IsSameAs(INI_CMD_JSON) )
            {
                m_kv_cmd_map[entry_key] = this->Read(this->GetPath() + "/" + entry_key).Trim();
            }
            else
                entry_vec.push_back(std::make_pair( entry_key, this->Read(this->GetPath() + "/" + entry_key).Trim() ));
        }

        bool ProcessJSON(wxJSONValue& json)
        {
            if ( json.HasMember("import") )
            {
                wxJSONValue import_list = json["import"];
                if ( import_list.IsArray() )
                {
                    for ( int i = 0; i< import_list.Size(); i++ )
                    {
                        wxString imp_value = import_list[i].AsString();
                        AddToQueue( m_import_que, imp_value );
                    }
                }
            }

            if ( json.HasMember("union") )
            {
                wxJSONValue union_list = json["union"];
                if ( union_list.IsArray() )
                {
                    for ( int i = 0; i < union_list.Size(); i++ )
                    {
                        wxString uni_value = union_list[i].AsString();
                        AddToQueue( m_union_que, uni_value );
                    }
                }
            }

            if ( json.HasMember("append") )
            {
                wxJSONValue append_item = json["append"];
                if ( append_item.IsObject() )
                {
                    wxArrayString members = append_item.GetMemberNames();
                    for (wxArrayString::const_iterator it = members.begin(); it != members.end(); ++it )
                    {
                        const wxString append_to = *it;
                        const wxString append_data = (append_item[*it]).AsString();

                        if ( m_opts_set.find(append_to) != m_opts_set.end() && m_parser->Found(append_to) )
                        {
                            m_kv_append_vec.push_back(std::make_pair(append_to, append_data));
                        }
                    }
                }
            }

            if ( json.HasMember("nvl") )
            {
                wxJSONValue nvl_item = json["nvl"];
                if ( nvl_item.IsObject() )
                {
                    wxArrayString members = nvl_item.GetMemberNames();
                    for (wxArrayString::const_iterator it = members.begin(); it != members.end(); ++it )
                    {
                        m_kv_nvl_map[*it]=(nvl_item[*it]).AsString();
                    }

                    m_nvl_que.push_back(m_kv_nvl_map);
                }
            }
        }


        bool JSONCommandProcessor()
        {
            wxStringToStringHashMap::const_iterator json_kv = m_kv_cmd_map.find(INI_CMD_JSON);

            if ( json_kv != m_kv_cmd_map.end() )
            {
                wxJSONValue root;
                if ( !ParseJSON((*json_kv).second, root) )
                    return false;

                if ( !ProcessJSON(root) )
                    return false;
            }

            return true;
        }

        size_t GetCfgEntriesVector( const wxString path, kvtype& group_entries )
        {
            wxString key;
            long idx;
            bool hasData(false);

            group_entries.clear();
            m_kv_cmd_map.clear();
            m_kv_nvl_map.clear();
            this->SetPath(path);
            size_t keyCnt = this->GetNumberOfEntries();

            if (keyCnt == 0)
                return 0;

            if (! this->GetFirstEntry(key, idx) )
                FilterEntries(group_entries, key);
            else
                hasData = true;


            while ( hasData )
            {
                FilterEntries(group_entries, key);
                hasData = this->GetNextEntry(key, idx);
            }

            JSONCommandProcessor();

            return keyCnt;
        }


        // works OK with simple imports;
        // works incorrectly for many cascaded imports;
        // different results when nesting 2 or more nvl commands
        void NvlTransform(kvtype& target_vec, bool fromback)
        {
            if ( m_nvl_transform && !m_nvl_que.empty() )
            {
                m_kv_nvl_map = (fromback)?m_nvl_que.back():m_nvl_que.front();

                for ( kvtype::iterator it = target_vec.begin(); it != target_vec.end(); ++it )
                {
                    const wxString& key = (*it).first;
                    wxString& val = (*it).second;

                    wxStringToStringHashMap::iterator mapit = m_kv_nvl_map.find(key);
                    if ( mapit != m_kv_nvl_map.end() )
                    {
                        const wxString keyup = "${" + key.Upper() + "}";

                        if ( val.IsEmpty() || val.IsSameAs(keyup) )
                        {
                            val = m_kv_nvl_map[key];
                            m_kv_nvl_map.erase(mapit);
                        }
                    }
                    //wxPuts(key + "=" + val);
                }

                (fromback)?m_nvl_que.pop_back():m_nvl_que.pop_front();
            }
        }

        // This takes care of iterator pointing to valid map entry,
        // so we can sequentially process cascading relationships
        void ManageCascadeCommands()
        {
            // fill cmd map with IMPORT key-val when queue is not empty and no IMPORT exists in cmd map
            // can't do this inside ProcessJSON because there could be IMPORT but no JSON key
            cascade_kv_iter = m_kv_cmd_map.find(INI_CMD_IMPORT);
            if ( cascade_kv_iter == m_kv_cmd_map.end() && !m_import_que.empty() )
            {
                m_kv_cmd_map[INI_CMD_IMPORT] = m_import_que.front();
                cascade_kv_iter = m_kv_cmd_map.find(INI_CMD_IMPORT);
                m_import_que.pop();
            }

            // NVL: if IMPORT exists, we must apply nvl after imported
            if ( m_nvl_que.size() > 1 || ( !m_nvl_que.empty() && cascade_kv_iter == m_kv_cmd_map.end() ) )
                m_nvl_transform = true;
            else
                m_nvl_transform = false;


            // UNION comes after IMPORT
            if ( cascade_kv_iter == m_kv_cmd_map.end() && m_import_que.empty() )
            {
                cascade_kv_iter = m_kv_cmd_map.find(INI_CMD_UNION);
                if ( cascade_kv_iter == m_kv_cmd_map.end() && !m_union_que.empty() )
                {
                    m_kv_cmd_map[INI_CMD_UNION] = m_union_que.front();
                    cascade_kv_iter = m_kv_cmd_map.find(INI_CMD_UNION);
                    m_union_que.pop();
                }
            }
        }



        bool ProcessGroups()
        {
            // m_parser = &parser; // used in ProcessJSON
            // loop and find target groups, then extract values (=build options: debug,release,test...)
            for ( wxArrayString::const_iterator grp = m_groups.begin(); grp != m_groups.end(); ++grp )
            {
                if ( this->HasGroup("/" + *grp) )
                {
                    bool foundGroupOption(false);

                    for (StringHashSet::const_iterator opt = m_opts_set.begin(); opt != m_opts_set.end(); ++opt)
                    {
                        if ( this->HasGroup("/" + *grp + "/" + *opt) && m_parser->Found(*opt) )
                        {
                            foundGroupOption = true;
                            this->GetCfgEntriesVector("/" + *grp + "/" + *opt, m_kv_vec);
                        }
                    }

                    if ( !foundGroupOption )
                        this->GetCfgEntriesVector("/" + *grp, m_kv_vec);

                    ManageCascadeCommands();
                    NvlTransform(m_kv_vec, false);

                    ///////////////////////////////////////////////////////////////////////
                    // The loop to process IMPORT and UNION entries
                    // that refer to other groups with IMPORT and UNION entris ...
                    ///////////////////////////////////////////////////////////////////////
                    size_t cyclic_cascade_fuse=0; // cyclic "import=" heuristic detection

                    while ( cascade_kv_iter != m_kv_cmd_map.end() )
                    {
                        bool doing_union = ((*cascade_kv_iter).first).IsSameAs(INI_CMD_UNION);

                        if ( cyclic_cascade_fuse++ > 255 )
                        {
                            wxPuts("cyclic_import_fuse triggered on group [" + *grp + "]\nAborting!");
                            return false;
                        }

                        if ( !this->HasGroup("/" + (*cascade_kv_iter).second ) )
                        {
                            wxPuts("Invalid import value in group [" + *grp + "]\nimport=" + (*cascade_kv_iter).second + "\nAborting!");
                            return false;
                        }

                        this->GetCfgEntriesVector(wxString("/" + (*cascade_kv_iter).second), m_kv_cascade_vec);

                        ManageCascadeCommands();

                        if ( doing_union )  // then add from m_kv_cascade_vec -> m_kv_vec
                        {
                            for ( kvtype::iterator it = m_kv_cascade_vec.begin(); it != m_kv_cascade_vec.end(); ++it )
                                m_kv_vec.push_back(std::make_pair((*it).first,(*it).second));
                        }
                        else // INI_CMD_IMPORT
                        {
                            m_kv_map.empty();
                            for ( kvtype::const_iterator it = m_kv_vec.begin(); it != m_kv_vec.end(); ++it )
                                m_kv_map[(*it).first] = (*it).second;


                            NvlTransform(m_kv_cascade_vec, true);


                            for ( kvtype::iterator it = m_kv_cascade_vec.begin(); it != m_kv_cascade_vec.end(); ++it )
                            {
                                const wxString& key = (*it).first;
                                wxString& val = (*it).second;

                                // Value in current group is preferred over value of the same key from "import="
                                // Here we replace values contained in "import" vector
                                wxStringToStringHashMap::iterator mapit = m_kv_map.find(key);
                                if ( mapit != m_kv_map.end() )
                                {
                                    val = m_kv_map[key];
                                    m_kv_map.erase(mapit);
                                }
                            }

                            // ... than we add missing values from current group to "import" vector
                            for ( kvtype::const_iterator it = m_kv_vec.begin(); it != m_kv_vec.end(); ++it )
                                if ( m_kv_map.find((*it).first) != m_kv_map.end() )
                                    m_kv_cascade_vec.push_back(std::make_pair((*it).first,(*it).second));





                            // swap vectors so content can be reused in next loop iteration (cascading "import" keys)
                            m_kv_vec.empty();
                            m_kv_vec.swap(m_kv_cascade_vec);
                        }
                    }

                    NvlTransform(m_kv_vec, false);


                    // now output all key-values pairs combined from groups
                    for ( kvtype::const_iterator it = m_kv_vec.begin(); it != m_kv_vec.end(); ++it )
                    {
                        m_final_list_vec.push_back(std::make_pair((*it).first,(*it).second));
                    }

                    // now output append data in reverse order (cascade command case)
                    for ( kvtype::reverse_iterator it = m_kv_append_vec.rbegin(); it != m_kv_append_vec.rend(); ++it )
                    {
                        m_final_list_vec.push_back(std::make_pair((*it).first,(*it).second));
                    }

                    m_kv_append_vec.clear();
                }
            }
        }
};


static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
    { wxCMD_LINE_SWITCH, "", "help" , "", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_PARAM, NULL, NULL, "csvparams", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL|wxCMD_LINE_PARAM_MULTIPLE  },
    { wxCMD_LINE_NONE }
};

bool ProcessCmdLine ( wxCmdLineParser& parser, xFileConfig& cfg )
{
    parser.SetDesc(g_cmdLineDesc);

    wxString sortlist;
    wxArrayString  groups_L1, groups_L2;    // Level 0 means root group "/"
    StringHashSet params;

    cfg.GetCfgGroupsArray("/", groups_L1);

    // extract sw target options from level 2 groups and make unique hashset
    for (wxArrayString::const_iterator grp = groups_L1.begin(); grp != groups_L1.end(); ++grp)
    {
        cfg.GetCfgGroupsArray("/" + *grp, groups_L2);
        // Level2 groups translate to command line options
        for (wxArrayString::const_iterator opt = groups_L2.begin(); opt != groups_L2.end(); ++opt)
        {
            cfg.m_opts_set.insert(*opt); // --debug --demo
        }
    }

    // add those options to parser switch set
    for (StringHashSet::const_iterator it = cfg.m_opts_set.begin(); it != cfg.m_opts_set.end(); ++it)
        parser.AddLongSwitch(*it);


    parser.SetLogo( "prefix---------------" + wxEOL );
    parser.AddUsageText(wxEOL + "suffix--------------" + wxEOL );

    switch (parser.Parse(true)) // "true" prints out Usage() when using invalid switch
    {
        case -1: // --help ali /h
            return false; // m_msg = parser.GetUsageString(); //break;

        case 0:

            // make hashset from parameters (group list)
            for (size_t i=0; i < parser.GetParamCount(); i++)
            {
                wxStringTokenizer tokenizer(parser.GetParam(i), ",");
                while ( tokenizer.HasMoreTokens() )
                    params.insert(tokenizer.GetNextToken());
            }

            cfg.m_parser = &parser;
            if ( cfg.HasGroup("/options") )
                cfg.m_groups.Add("options");

            // create sort array from config file "sort" entry and parameters
            if ( cfg.HasEntry("/sort") && cfg.Read("/sort", &sortlist) )
            {
                wxStringTokenizer tokenizer(sortlist, ",");
                while ( tokenizer.HasMoreTokens() )
                {
                    wxString token = tokenizer.GetNextToken();
                    if ( params.find(token) != params.end() )
                        cfg.m_groups.Add(token);
                }
            }

            // add parameters from command line if not in sortlist
            for (StringHashSet::const_iterator par = params.begin(); par != params.end(); ++par)
                if ( cfg.m_groups.Index(*par) == wxNOT_FOUND )
                    cfg.m_groups.Add(*par);

            return cfg.ProcessGroups();

            break;

        default:
            return false;
    }

    return true ;
}

wxString AppendSpaceIfNotEmpty(const wxString& data)
{
    return data.IsEmpty()?"":data + " ";
}

int main(int argc, char **argv)
{
    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "y-config");
    wxInitializer initializer;
    if ( !initializer )
    {
        fprintf(stderr, "Failed to initialize the wxWidgets library, aborting.");
        return -1;
    }

    wxStandardPaths path = wxStandardPaths::Get();
    wxString inifname = wxFileName(path.GetExecutablePath()).GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR).Append(wxTheApp->GetAppName()).Append(".ini");

    xFileConfig config(inifname);

    // in case those entries are missing under /options group
    config.m_opts_set.insert("cc");
    config.m_opts_set.insert("cxx");
    config.m_opts_set.insert("cxxflags");
    config.m_opts_set.insert("ldflags");
    config.m_opts_set.insert("libs");
    config.m_opts_set.insert("release");

    wxCmdLineParser parser( argc, argv);

    if (!ProcessCmdLine(parser, config))
        return -1;

    wxString cc, cxx, cxxflags, ldflags, libs, appenddata;

    for (kvtype::const_iterator it = config.m_final_list_vec.begin(); it != config.m_final_list_vec.end(); ++it)
    {
        const wxString& key = (*it).first;
        const wxString& val = (*it).second;

        if ( parser.Found("cc") && key.IsSameAs("cc") && !val.IsEmpty() && !val.IsSameAs("${CC}") )
            cc.Append(AppendSpaceIfNotEmpty(val));

        else if ( parser.Found("cxx") && key.IsSameAs("cxx") && !val.IsEmpty() && !val.IsSameAs("${CXX}") )
            cxx.Append(AppendSpaceIfNotEmpty(val));

        else if ( parser.Found("cxxflags") && key.IsSameAs("cxxflags") && !val.IsEmpty() && !val.IsSameAs("${CXXFLAGS}") )
            cxxflags.Append(AppendSpaceIfNotEmpty(val));

        else if ( parser.Found("ldflags") && key.IsSameAs("ldflags") && !val.IsEmpty() )
            ldflags.Append(AppendSpaceIfNotEmpty(val));

        else if ( parser.Found("libs") && key.IsSameAs("libs") && !val.IsEmpty() )
            libs.Append(AppendSpaceIfNotEmpty(val));
    }

    for ( wxArrayString::const_iterator it = config.m_global_appends.begin(); it != config.m_global_appends.end(); ++it )
        appenddata.Append(AppendSpaceIfNotEmpty(*it));

    wxPuts(AppendSpaceIfNotEmpty(cc) + AppendSpaceIfNotEmpty(cxx) + cxxflags + ldflags + libs + appenddata);
    return 0;
}

