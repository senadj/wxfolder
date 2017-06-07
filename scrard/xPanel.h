#pragma once
#include <wx/panel.h>
#include <wx/dynarray.h>
#include <wx/textctrl.h>
//class wxTextCtrl;

WX_DEFINE_ARRAY(wxObject*, wxObjectArray);

class GridLineMeta : public wxObject
{
public:
    wxObjectArray pobjs;
    int pin;
};

WX_DECLARE_STRING_HASH_MAP(int, StringIntHashMap);

class xPanel : public wxPanel
{
public:
    wxTextCtrl* m_tctl;
	xPanel(wxWindow* parent);
	~xPanel();
    void OnClick(wxCommandEvent& evt);
    void OnClickToggleButton(wxCommandEvent& evt);
    void OnScroll(wxCommandEvent& evt);
    void ProcessToggle(GridLineMeta* pline /* source row line */ , int sidx /* source column index */, bool bval /* button value */ );
    void WindowPopDisable(GridLineMeta* pGridLine /* source row line */, int pTargetColumnIndex /* column index */);
    void WindowEnable(GridLineMeta* pline /* source row line */, int cidx /* column index */, bool enabled );
    void TogglePop(GridLineMeta* pGridLine /* source row line */, int pTargetColumnIndex /* column index */);
    void DisableOuts( GridLineMeta* pGridLine );
    void DisableIns( GridLineMeta* pGridLine );
    void OnSerialUpdate(wxString& pData);
    bool IsAnalogReadable(int pin);
    bool IsAnalogWriteable(int pin);
    void TogglePush(GridLineMeta& pline,wxString bname);
    void SetOutputFromExternal(int pin, int value);
    void LedON(GridLineMeta* pline);
    void LedOFF(GridLineMeta* pline);
    void SetInputFromExternal(int pin, char state); // d | a




//protected:
    wxArrayString m_cols;
    GridLineMeta* m_pins;
    StringIntHashMap m_mapc; // columns map
};
