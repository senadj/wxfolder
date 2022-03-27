#include "xApp.h"
#include "xPanel.h"
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/tglbtn.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include <wx/slider.h>
#include <wx/arrstr.h>
#include <wx/msgdlg.h>
#include <wx/hashset.h>
#include <wx/clntdata.h>
#include <wx/tokenzr.h>
#include <wx/awx/led.h>

xPanel::~xPanel() { wxDELETEA(m_pins); }

// don't put in header!
WX_DECLARE_HASH_SET( int, wxIntegerHash, wxIntegerEqual, IntegerHashSetType );
static IntegerHashSetType anReadPins;
static IntegerHashSetType anWritePins;
static IntegerHashSetType customWritePins;
bool xPanel::IsAnalogReadable(int pin) {return anReadPins.find(pin) != anReadPins.end();}
bool xPanel::IsAnalogWriteable(int pin) {return anWritePins.find(pin) != anWritePins.end();}
bool xPanel::IsCustomWriteable(int pin) {return customWritePins.find(pin) != customWritePins.end();}

void xPanel::TogglePush(GridLineMeta& pline,wxString bname)
{
    wxToggleButton* btn = wxDynamicCast(pline.pobjs.Item(m_mapc[bname]),wxToggleButton);
    if(btn)
    {
        btn->SetValue(true);
        ProcessToggle(&pline,m_mapc[bname],true);
    }
}

void xPanel::LedON(GridLineMeta* pline)
{
    if ( wxGetApp().IsScratchConnected() )
    {
        wxStaticCast(pline->pobjs.Item(m_mapc["external"]),awxLed)->SetState( awxLED_ON );
        //awxLed* awxled = wxDynamicCast(pline->pobjs.Item(m_mapc["external"]),awxLed);
        //if (awxled) awxled->SetState( awxLED_ON );
    }
}

void xPanel::LedOFF(GridLineMeta* pline)
{
    wxStaticCast(pline->pobjs.Item(m_mapc["external"]),awxLed)->SetState( awxLED_OFF );
    //awxLed* awxled = wxDynamicCast(pline->pobjs.Item(m_mapc["external"]),awxLed);
    //if (awxled) awxled->SetState( awxLED_OFF );
}

void xPanel::HideRow(int pin)
{
    GridLineMeta& line = m_pins[pin];
    for (int i=0; i<line.pobjs.Count(); i++)
    {
        wxWindow* win = wxDynamicCast(line.pobjs.Item(i),wxWindow);
        if (win) win->Hide();
    }

}


void xPanel::SetInputFromExternal(int pin, char state)  // called from scratch using broadcast: analogRead(10) | digitalRead(15)
{
    GridLineMeta line = m_pins[pin];

    TogglePush(line, "INPUT");

    if ( state == 'D' )
    {
        TogglePush(line, "digitalRead");
    }
    else if ( state == 'A' )
    {
        TogglePush(line, "analogRead");
    }
    else if ( state == 'd' )
    {
        DisableIns(&line);
        LedOFF(&line);
    }
    else if ( state == 'a' )
    {
        DisableIns(&line);
        LedOFF(&line);
    }
}

void xPanel::SetOutputFromExternal(int pin, int value)
{
    GridLineMeta& line = m_pins[pin];

    if ( wxGetApp().m_writetype4arduino[pin] == UNCONFIGURED ) // arduino D/A write check
    {
        TogglePush(line, "OUTPUT");

        if ( IsAnalogWriteable(pin) )
        {
            TogglePush(line, "analogWrite");
            wxGetApp().m_writetype4arduino[pin] = ANALOGWRITE;
        }
        else
        {
            TogglePush(line, "digitalWrite");
            wxGetApp().m_writetype4arduino[pin] = DIGITALWRITE;
        }


        awxLed* awxtcp = wxStaticCast(line.pobjs.Item(m_mapc["external"]),awxLed);
        if (awxtcp)
        {
            awxtcp->SetColour( awxLED_YELLOW );
            awxtcp->SetState( awxLED_ON );
        }
    }

    if ( wxGetApp().m_writetype4arduino[pin] == ANALOGWRITE || wxGetApp().m_writetype4arduino[pin] == CUSTOMWRITE )
    {
        wxSlider* slider = wxDynamicCast(line.pobjs.Item(m_mapc["analogWriteValue"]),wxSlider);
        if (slider)
            slider->SetValue(value);
    }
    else
    {
        wxToggleButton* btn = wxDynamicCast(line.pobjs.Item(m_mapc["HIGH"]),wxToggleButton);
        if (btn)
            btn->SetValue(value);

        btn = wxDynamicCast(line.pobjs.Item(m_mapc["LOW"]),wxToggleButton);
        if (btn)
            btn->SetValue(!value);
    }
}

void xPanel::SetAlias(int pin)
{
    wxStaticCast(m_pins[pin].pobjs.Item(m_mapc["alias"]),wxStaticText)->SetLabelText(wxGetApp().m_pin2alias[pin]);
}

xPanel::xPanel(wxWindow* parent) : wxPanel(parent)
{
	m_cols.Add("pin");
	m_cols.Add("INPUT");
	m_cols.Add("INPUT_PULLUP");
	m_cols.Add("digitalRead");
	m_cols.Add("analogRead");
    m_cols.Add("ReadValue");
	m_cols.Add("OUTPUT");
	m_cols.Add("digitalWrite");
	m_cols.Add("LOW");
	m_cols.Add("HIGH");
	m_cols.Add("analogWrite");
	m_cols.Add("analogWriteValue");
	m_cols.Add("external");
	m_cols.Add("alias");

	// fill column to int map
    for (int k=0; k < m_cols.GetCount(); k++)
        m_mapc[m_cols[k]] = k;

    // Uno
    for (auto it: wxSplit( wxGetApp().m_cfg_mgr->ReadEntry("/analogRead","14,15,16,17,18,19"),','))
        anReadPins.insert(std::stoi(it.ToStdString()));

    // PWM
    for (auto it: wxSplit( wxGetApp().m_cfg_mgr->ReadEntry("/analogWrite","3,5,6,9,10,11"),','))
        anWritePins.insert(std::stoi(it.ToStdString()));

    for (int cpin=0; cpin < wxGetApp().m_pincnt; cpin++)
        if ( wxGetApp().m_inipincfg[cpin].IsSameAs("CUSTOM") || wxGetApp().m_inipincfg[cpin].IsSameAs("SERVO") )
            customWritePins.insert(cpin);


    wxBoxSizer* vboxsz = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* sizer = new wxFlexGridSizer( m_cols.GetCount(), 0, 0 );
    m_pins = new GridLineMeta[ wxGetApp().m_pincnt ];

    wxSize basePinFieldSize;

	for ( int i=0; i < wxGetApp().m_pincnt; i++ )
    {
        m_pins[i].pin = i;
        wxString& inipincfg = wxGetApp().m_inipincfg[i];
        wxString& inipincmd = wxGetApp().m_inipincmd[i];

        for ( int j=0; j < sizer->GetCols(); j++ )
        {
            wxString& ccol = m_cols[j]; // current column
            wxString textlabel = ccol;
            wxString createwidget = wxEmptyString;

            if ( ccol.IsSameAs("pin") )
            {
                wxStaticText* text = new wxStaticText (this,wxID_ANY,"pin" + wxString::Format("%i",i),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE_HORIZONTAL);
                basePinFieldSize.SetWidth(text->GetSize().GetWidth() * 1.4);
                basePinFieldSize.SetHeight(text->GetSize().GetHeight());
                text->SetClientData((void*)&m_pins[i]);
                m_pins[i].pobjs.Add((wxObject*)text);
                sizer->Add(text,wxSizerFlags().Expand().Center().Border(wxALL,5).FixedMinSize());
            }
            else if ( IsCustomWriteable(i) &&
                       ! ( ccol.IsSameAs("analogWrite")
                        || ccol.IsSameAs("analogWriteValue")
                        || ccol.IsSameAs("external")
                        || ccol.IsSameAs("alias") ) )
            {
                createwidget = "Window";
            }
            else if ( ccol.IsSameAs("INPUT") || ccol.IsSameAs("INPUT_PULLUP") || ccol.IsSameAs("OUTPUT") )
            {
                if (  !inipincfg.IsEmpty() && !ccol.IsSameAs(inipincfg) ) createwidget = "Window";
                else if ( ccol.IsSameAs(inipincfg) ) createwidget = "Button";
                else createwidget = "ToggleButton";
            }
            else if ( ccol.IsSameAs("digitalRead") || ccol.IsSameAs("analogRead") )
            {
                if (  !inipincfg.IsEmpty() && !inipincfg.StartsWith("INPUT") ) createwidget = "Window";
                else if ( ccol.IsSameAs("analogRead") && !IsAnalogReadable(i) ) createwidget = "Window";
                else if ( ccol.IsSameAs("analogRead") && inipincmd.IsSameAs("digitalRead") ) createwidget = "Window";
                else if ( ccol.IsSameAs("digitalRead") && inipincmd.IsSameAs("analogRead") ) createwidget = "Window";
                else if ( ccol.IsSameAs(inipincmd) ) createwidget = "Button";
                else createwidget = inipincfg.IsEmpty() ? "ToggleButtonDisable" : "ToggleButton";
            }
            else if ( ccol.IsSameAs("ReadValue") )
            {
                textlabel = '-';
                if (  !inipincfg.IsEmpty() && !inipincfg.StartsWith("INPUT") ) createwidget = "Window";
                else if ( inipincmd.Find("Read") != wxNOT_FOUND ) createwidget = "StaticText Resize";
                else createwidget = "StaticText Disable Resize";
            }
            else if ( ccol.IsSameAs("digitalWrite") )
            {
                if (  !inipincfg.IsEmpty() && !( inipincfg.IsSameAs("OUTPUT") ) ) createwidget = "Window";
                else if ( inipincmd.IsSameAs("analogWrite") ) createwidget = "Window";
                else if ( ccol.IsSameAs(inipincmd) ) createwidget = "Button";
                else createwidget = inipincfg.IsEmpty() ? "ToggleButtonDisable" : "ToggleButton";
            }
            else if ( ccol.IsSameAs("LOW") || ccol.IsSameAs("HIGH") )
            {
                if (  !inipincfg.IsEmpty() && !( inipincfg.IsSameAs("OUTPUT") ) ) createwidget = "Window";
                else if ( inipincmd.IsSameAs("analogWrite") ) createwidget = "Window";
                else if ( inipincmd.IsSameAs("digitalWrite") )
                {
                    if ( ccol.IsSameAs("LOW") && wxGetApp().m_inipinval[i] == 0 ) createwidget = "ToggleButton True";
                    else if ( ccol.IsSameAs("HIGH") && wxGetApp().m_inipinval[i] == 1 ) createwidget = "ToggleButton True";
                    else createwidget = "ToggleButton";
                }
                else createwidget = inipincfg.IsEmpty() ? "ToggleButtonDisable" : inipincmd.IsEmpty() ? "ToggleButtonDisable" : "ToggleButton";
            }
            else if ( ccol.IsSameAs("analogWrite") )
            {
                if ( IsCustomWriteable(i) ) { textlabel = inipincmd.IsEmpty() ? "[ ? ]" : inipincmd; createwidget = "StaticText"; }
                else if (  !inipincfg.IsEmpty() && !( inipincfg.IsSameAs("OUTPUT") ) ) createwidget = "Window";
                else if ( !IsAnalogWriteable(i) || inipincmd.IsSameAs("digitalWrite") ) createwidget = "Window";
                else if ( ccol.IsSameAs(inipincmd) ) createwidget = "Button";
                else createwidget = inipincfg.IsEmpty() ? "ToggleButtonDisable" : "ToggleButton";
            }
            else if ( ccol.IsSameAs("analogWriteValue") )
            {
                if ( IsAnalogWriteable(i) || IsCustomWriteable(i) )
                {
                    int smin = 0;
                    int smax = 255;
                    if ( wxGetApp().m_slider_limits.find(i) != wxGetApp().m_slider_limits.end() )
                    {
                        smin = wxGetApp().m_slider_limits[i].first;
                        smax = wxGetApp().m_slider_limits[i].second;
                    }
                    wxSlider* slider = new wxSlider (this,wxID_ANY, 0, smin, smax, wxDefaultPosition, wxSize(170,44), wxSL_HORIZONTAL|wxSL_LABELS);
                    slider->SetLineSize(255);
                    slider->SetClientData((void*)&m_pins[i]);
                    m_pins[i].pobjs.Add((wxObject*)slider);

                    slider->Bind(wxEVT_SLIDER, &xPanel::OnScroll, this);
                    sizer->Add(slider,wxALIGN_TOP);

                    if ( wxGetApp().m_inipinval[i] > 0 )
                        slider->SetValue(wxGetApp().m_inipinval[i]);

                    if ( !IsCustomWriteable(i) && !inipincmd.IsSameAs("analogWrite") )
                        slider->Disable();
                }
                else
                    createwidget = "Window";
            }
            else if ( ccol.IsSameAs("alias"))
            {
                textlabel = wxGetApp().m_pin2alias[i]; // "ho"; // + wxString::Format("%d",i);
                createwidget = "StaticText Resize";
                //createwidget = "Button";
            }
            else if ( ccol.IsSameAs("external") )
            {
                awxLed* awx = new awxLed( this, wxID_ANY );
                awx->SetColour( awxLED_YELLOW );
                awx->SetState( awxLED_OFF );
                awx->SetClientData((void*)&m_pins[i]);
                m_pins[i].pobjs.Add((wxObject*)awx);
                //sizer->Add(awx,1,wxALL|wxEXPAND,5);
                sizer->Add(awx,wxSizerFlags().Expand().Center());
            }
            else
                //sizer->Add(new wxWindow()); m_pins[i].pobjs.Add((wxObject*)NULL);
                wxLogError("error" + ccol); // )if ( createwidget.IsEmpty() )                     sizer->Add(new wxWindow());
            //tb->Bind(wxEVT_TOGGLEBUTTON, &xPanel::OnClickToggleButton, this);
            //tb->Bind(wxEVT_TOGGLEBUTTON, &OnClickToggleButton, wxID_ANY, wxID_ANY, (wxObject*)glm);
            //sizer->Add(tb);

            if ( createwidget.IsSameAs("Window") )
            {
                sizer->Add(new wxWindow()); m_pins[i].pobjs.Add((wxObject*)NULL);
            }
            else if ( createwidget.StartsWith("ToggleButton") )
            {
                wxToggleButton* tbutton = new wxToggleButton (this,wxID_ANY,ccol);
                tbutton->SetClientData((void*)&m_pins[i]);
                m_pins[i].pobjs.Add((wxObject*)tbutton);
                sizer->Add(tbutton,wxSizerFlags().Expand().Center());
                if ( createwidget.Find("True") != wxNOT_FOUND )
                    tbutton->SetValue(true);
                if ( createwidget.Find("Disable") != wxNOT_FOUND )
                    tbutton->Disable();
            }
            else if ( createwidget.StartsWith("StaticText") )
            {
                wxStaticText* tctl = new wxStaticText (this,wxID_ANY,textlabel,wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE_HORIZONTAL);
                if ( createwidget.Find("Resize") != wxNOT_FOUND )
                    tctl->SetSize(wxSize(basePinFieldSize.GetWidth(), tctl->GetSize().GetHeight()));
                tctl->SetClientData((void*)&m_pins[i]);
                m_pins[i].pobjs.Add((wxObject*)tctl);
                sizer->Add(tctl,wxSizerFlags().Expand().Center().FixedMinSize());
                if ( createwidget.Find("Disable") != wxNOT_FOUND )
                    tctl->Disable();
            }
            else if ( createwidget.IsSameAs("Button") )
            {
                wxButton* button = new wxButton (this,wxID_ANY,ccol);
                button->SetForegroundColour(*wxBLACK);
                button->SetBackgroundColour(*wxWHITE);
                button->SetClientData((void*)&m_pins[i]);
                m_pins[i].pobjs.Add((wxObject*)button);
                sizer->Add(button,wxSizerFlags().Expand().Center());
                button->Disable();
            }
        }

        Bind( wxEVT_TOGGLEBUTTON, &xPanel::OnClickToggleButton, this );
        Bind( wxEVT_BUTTON, &xPanel::OnClick, this );
        //Bind( wxEVT_SLIDER, &xPanel::OnScroll, this );
    }

    for (auto it: wxSplit( wxGetApp().m_hidepins,','))
        HideRow(std::stoi(it.ToStdString()));

	m_tctl = new wxTextCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize, wxTE_MULTILINE | wxTE_RICH);
	delete wxLog::SetActiveTarget(new wxLogTextCtrl(m_tctl));

	vboxsz->Add(sizer,1,wxEXPAND);
	//vboxsz->Add(new wxButton(this,wxID_ANY,"click"),1,wxEXPAND);
	vboxsz->Add(m_tctl,1,wxEXPAND);

	this->SetSizer(vboxsz);
}


void xPanel::OnSerialUpdate(wxString& s)
{
    long int pinNo, pinVal, msglen;
    size_t  slen = s.length()-1; // minus newline
    const char* p = s.c_str();
    char* pend; // after strtol used for validation from hex to number
    char cbuff2[3]  = {'?','?','\0'}; // used for conversion of hex value to int using strtol
    char cbuff3[4]  = {'?','?','?','\0'}; // used for conversion of hex value to int using strtol

    if (slen == 0) return;
    if ( *(p+slen-1) == '\r' )
    {
        m_tctl->AppendText(s);
        return;
    }

    if (slen < 7)
        return;
    else
        slen-=2;

    msglen = strtol(p+slen,&pend,16); // this should be msg length hex encoded

    if (msglen%5!=0) return;
    if (slen<msglen) return; // missing beginning of msg?

    if (slen>msglen)  // serial buffer garbage?
    {
        p+=slen-msglen;
    }

    for ( long int i = 0; i < msglen/5; i++ )
    {
        memcpy(&cbuff2[0],p,2);
        pinNo = strtol(cbuff2,&pend,16);
        if (*pend) { m_tctl->AppendText("error\n"); return; } // hex to num conversion error
        p+=2;

        memcpy(&cbuff3[0],p,3);
        pinVal = strtol(cbuff3,&pend,16);
        if (*pend) { m_tctl->AppendText("error\n"); return; } // hex to num conversion error
        p+=3;

        if ( pinNo==0xFF && pinVal==0xFFF)
        {
            wxGetApp().m_ping = true;
            return;
        }

        //m_tctl->AppendText(wxString::Format("%i", pinNo) + " " + wxString::Format("%i", pinVal) + '\n');
        wxObject* obj = m_pins[pinNo].pobjs[m_mapc["ReadValue"]];
        wxStaticText* stat = wxDynamicCast( obj, wxStaticText );
        if (stat) stat->SetLabel(wxString::Format("%i", pinVal));

        if ( wxGetApp().m_buff4scratch[pinNo] != pinVal )
        {
            wxGetApp().m_buff4scratch[pinNo] = pinVal;
            wxGetApp().m_flag4scratch[pinNo] = true;
        }
    }
}


void xPanel::TogglePop(GridLineMeta* pGridLine /* source row line */, int pTargetColumnIndex /* column index */)
{
    if ( pTargetColumnIndex != wxNOT_FOUND )
    {
        wxToggleButton* btn = wxDynamicCast( pGridLine->pobjs.Item(pTargetColumnIndex), wxToggleButton );
        if (btn) btn->SetValue(false);
    }
}

void xPanel::WindowPopDisable(GridLineMeta* pGridLine /* source row line */, int pTargetColumnIndex /* column index */)
{
    if ( pTargetColumnIndex != wxNOT_FOUND )
    {
        wxToggleButton* btn = wxDynamicCast( pGridLine->pobjs.Item(pTargetColumnIndex), wxToggleButton );
        if (btn)
        {
            btn->SetValue(false);
            btn->Disable();
        }
        else
        {
            wxWindow* win = wxDynamicCast( pGridLine->pobjs.Item(pTargetColumnIndex), wxWindow);
            if (win) win->Disable();
        }
    }
}

void xPanel::WindowEnable(GridLineMeta* pGridLine /* source row line */, int pTargetColumnIndex /* column index */, bool enabled )
{
    if ( pTargetColumnIndex != wxNOT_FOUND )
    {
        wxWindow* win = wxDynamicCast( pGridLine->pobjs.Item(pTargetColumnIndex), wxWindow);
        if (win) win->Enable(enabled);
    }
}

void xPanel::DisableOuts( GridLineMeta* pGridLine )
{
    WindowPopDisable(pGridLine,m_mapc["digitalWrite"]);
    WindowPopDisable(pGridLine,m_mapc["LOW"]);
    WindowPopDisable(pGridLine,m_mapc["HIGH"]);
    if (IsAnalogWriteable(pGridLine->pin)) WindowPopDisable(pGridLine,m_mapc["analogWrite"]);
    if (IsAnalogWriteable(pGridLine->pin)) WindowPopDisable(pGridLine,m_mapc["analogWriteValue"]);
    LedOFF(pGridLine);
}

void xPanel::DisableIns( GridLineMeta* pGridLine )
{
    WindowPopDisable(pGridLine,m_mapc["digitalRead"]);
    WindowPopDisable(pGridLine,m_mapc["ReadValue"]);
    if (IsAnalogReadable(pGridLine->pin)) WindowPopDisable(pGridLine,m_mapc["analogRead"]);
    LedOFF(pGridLine);
}


void xPanel::ProcessToggle(GridLineMeta* pline /* source row line */ , int sidx /* source column index */, bool bval /* button value */ )
{
    int tidx;
    wxString& colname = m_cols[sidx];

    if ( colname.IsSameAs("INPUT") || colname.IsSameAs("INPUT_PULLUP") ) // && bval
    {
        awxLed* ledExt = wxStaticCast(pline->pobjs.Item(m_mapc["external"]),awxLed);

        wxGetApp().m_writetype4arduino[pline->pin] = UNCONFIGURED;
        wxGetApp().m_buff4arduino[pline->pin] = wxINT32_MIN;

        if (!bval)
        {
            DisableIns(pline);
            if (ledExt) ledExt->SetState( awxLED_OFF );
            wxGetApp().ArduinoCommand(pline->pin, 'i');
        }
        else
        {
            TogglePop(pline,m_mapc["OUTPUT"]);
            TogglePop(pline, colname.IsSameAs("INPUT")?m_mapc["INPUT_PULLUP"]:m_mapc["INPUT"]);

            WindowEnable(pline,m_mapc["digitalRead"], bval);
            if (IsAnalogReadable(pline->pin)) WindowEnable(pline,m_mapc["analogRead"], bval);

            DisableOuts(pline);
            if (ledExt) ledExt->SetColour( awxLED_GREEN );
            wxGetApp().ArduinoCommand(pline->pin, colname.IsSameAs("INPUT")?'I':'P');
        }
    }
    else if ( colname.IsSameAs("OUTPUT") )
    {
        awxLed* ledExt = wxStaticCast(pline->pobjs.Item(m_mapc["external"]),awxLed);

        if (!bval)
        {
            DisableOuts(pline);
            wxGetApp().m_writetype4arduino[pline->pin] = UNCONFIGURED;
            wxGetApp().m_buff4arduino[pline->pin] = wxINT32_MIN;
        }
        else
        {
            TogglePop(pline,m_mapc["INPUT"]);
            TogglePop(pline,m_mapc["INPUT_PULLUP"]);

            WindowEnable(pline,m_mapc["digitalWrite"], bval);
            if (IsAnalogWriteable(pline->pin)) WindowEnable(pline,m_mapc["analogWrite"], bval);

            DisableIns(pline);
            if (ledExt) ledExt->SetColour( awxLED_YELLOW );
            wxGetApp().ArduinoCommand(pline->pin, 'O');
        }
    }
    else if ( colname.IsSameAs("digitalRead") )
    {
        if (bval)
        {
            WindowEnable(pline,m_mapc["ReadValue"], bval);
            TogglePop(pline,m_mapc["analogRead"]);
            LedON(pline);
            wxGetApp().ArduinoCommand(pline->pin, 'D');
        }
        else
        {
            WindowEnable(pline,m_mapc["ReadValue"], false);
            LedOFF(pline);
            wxGetApp().ArduinoCommand(pline->pin, 'd');
        }

    }
    else if ( colname.IsSameAs("analogRead") )
    {
        if (bval)
        {
            WindowEnable(pline,m_mapc["ReadValue"], bval);
            TogglePop(pline,m_mapc["digitalRead"]);
            LedON(pline);
            wxGetApp().ArduinoCommand(pline->pin, 'A');
        }
        else
        {
            WindowEnable(pline,m_mapc["ReadValue"], false);
            LedOFF(pline);
            wxGetApp().ArduinoCommand(pline->pin, 'a');
        }

    }
    else if ( colname.IsSameAs("digitalWrite") )
    {
        if (bval)
        {
            if (IsAnalogWriteable(pline->pin))
            {
                TogglePop(pline,m_mapc["analogWrite"]);
                WindowPopDisable(pline,m_mapc["analogWriteValue"]);
            }

            WindowEnable(pline,m_mapc["LOW"], bval);
            WindowEnable(pline,m_mapc["HIGH"], bval);
            if ( wxGetApp().m_writetype4arduino[pline->pin] == ANALOGWRITE )
            {
                wxGetApp().m_writetype4arduino[pline->pin] = DIGITALWRITE;
                wxGetApp().m_buff4arduino[pline->pin] = wxINT32_MIN;
            }
            wxGetApp().ArduinoCommand(pline->pin, 'W');
        }
        else
        {
            WindowPopDisable(pline,m_mapc["LOW"]);
            WindowPopDisable(pline,m_mapc["HIGH"]);
        }
    }
    else if ( colname.IsSameAs("analogWrite") )
    {
        if (bval)
        {
            TogglePop(pline,m_mapc["digitalWrite"]);
            WindowPopDisable(pline,m_mapc["LOW"]);
            WindowPopDisable(pline,m_mapc["HIGH"]);
            WindowEnable(pline,m_mapc["analogWriteValue"], bval);
            if ( wxGetApp().m_writetype4arduino[pline->pin] == DIGITALWRITE )
            {
                wxGetApp().m_writetype4arduino[pline->pin] = ANALOGWRITE;
                wxGetApp().m_buff4arduino[pline->pin] = wxINT32_MIN;
            }
            wxGetApp().ArduinoCommand(pline->pin, 'U');
        }
        else
        {
            WindowEnable(pline,m_mapc["analogWriteValue"], false);
        }
    }
    else if ( colname.IsSameAs("LOW")  )
    {
        wxToggleButton* btn = wxDynamicCast(pline->pobjs.Item(sidx), wxToggleButton);
        // ??????????
        //wxToggleButton* b01 = wxDynamicCast(pline->pobjs.Item(m_mapc["INPUT"]), wxToggleButton);
        //if ( b01 && b01->GetValue() )
        //    btn->SetValue(true);
        //else
        {
            wxToggleButton* bhigh = wxDynamicCast(pline->pobjs.Item(m_mapc["HIGH"]), wxToggleButton);
            if (bhigh) bhigh->SetValue(false);
        }
        wxGetApp().ArduinoPinData(pline->pin, btn->GetValue()?0:1);
    }
    else if ( colname.IsSameAs("HIGH") )
    {
        wxToggleButton* btn = wxDynamicCast(pline->pobjs.Item(sidx), wxToggleButton);
        // ??????????
        //wxToggleButton* b01 = wxDynamicCast(pline->pobjs.Item(m_mapc["INPUT"]), wxToggleButton);
        //if ( b01 && b01->GetValue() )
        //    btn->SetValue(true);
        //else
        {
            wxToggleButton* blow = wxDynamicCast(pline->pobjs.Item(m_mapc["LOW"]), wxToggleButton);
            if (blow) blow->SetValue(false);
        }
        wxGetApp().ArduinoPinData(pline->pin, btn->GetValue()?1:0);
    }
}

void xPanel::OnScroll(wxCommandEvent& evt)
{
    wxObject* obj = evt.GetEventObject();
    wxSlider* slider = wxDynamicCast(obj, wxSlider);

    if (slider)
    {
        GridLineMeta* pline = (GridLineMeta*)slider->GetClientData();
        wxGetApp().ArduinoPinData(pline->pin, slider->GetValue());
    }
}

void xPanel::OnClickToggleButton(wxCommandEvent & evt)
{
    wxObject* obj = evt.GetEventObject();
    wxToggleButton* sourceToggleButton = wxDynamicCast(obj, wxToggleButton);

    if (sourceToggleButton)
    {
        GridLineMeta* pGridLine = (GridLineMeta*)sourceToggleButton->GetClientData(); // pline->pin = button row number
        int sourceButtonIndex = pGridLine->pobjs.Index(obj);    // = button column number
        if ( sourceButtonIndex != wxNOT_FOUND )
        {
            ProcessToggle(pGridLine, sourceButtonIndex, sourceToggleButton->GetValue());
        }
    }
}

void xPanel::OnClick(wxCommandEvent& evt)
{
    //wxGetApp().Test();
}


