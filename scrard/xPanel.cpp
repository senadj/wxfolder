#include "xApp.h"
#include "xPanel.h"
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/tglbtn.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/stattext.h>
#include <wx/slider.h>
#include <wx/arrstr.h>
#include <wx/msgdlg.h>
#include <wx/hashset.h>
#include <wx/clntdata.h>
#include <wx/awx/led.h>

xPanel::~xPanel() { wxDELETEA(m_pins); }

// don't put in header!
WX_DECLARE_HASH_SET( int, wxIntegerHash, wxIntegerEqual, IntegerHashSetType );
static IntegerHashSetType anReadPins;
static IntegerHashSetType anWritePins;
bool xPanel::IsAnalogReadable(int pin) {return anReadPins.find(pin) != anReadPins.end();}
bool xPanel::IsAnalogWriteable(int pin) {return anWritePins.find(pin) != anWritePins.end();}

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

    if ( wxGetApp().m_writetype4arduino[pin] == ANALOGWRITE )
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
    wxStaticCast(m_pins[pin].pobjs.Item(m_mapc["alias"]),wxStaticText)->SetLabelText(wxGetApp().m_pin2alias[pin]);;
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
    anReadPins.insert(14);
    anReadPins.insert(15);
    anReadPins.insert(16);
    anReadPins.insert(17);
    anReadPins.insert(18);
    anReadPins.insert(19);

    // PWM
    anWritePins.insert(3);
    anWritePins.insert(5);
    anWritePins.insert(6);
    anWritePins.insert(9);
    anWritePins.insert(10);
    anWritePins.insert(11);

/*
    //MEGA     app.m_pincnt=70

    anReadPins.insert(54);
    anReadPins.insert(55);
    anReadPins.insert(56);
    anReadPins.insert(57);
    anReadPins.insert(58);
    anReadPins.insert(59);
    anReadPins.insert(60);
    anReadPins.insert(61);
    anReadPins.insert(62);
    anReadPins.insert(63);
    anReadPins.insert(64);
    anReadPins.insert(65);
    anReadPins.insert(66);
    anReadPins.insert(67);
    anReadPins.insert(68);
    anReadPins.insert(69);


    anWritePins.insert(2);
    anWritePins.insert(3);
    anWritePins.insert(4);
    anWritePins.insert(5);
    anWritePins.insert(6);
    anWritePins.insert(7);
    anWritePins.insert(8);
    anWritePins.insert(9);
    anWritePins.insert(10);
    anWritePins.insert(11);
    anWritePins.insert(12);
    anWritePins.insert(13);
    anWritePins.insert(44);
    anWritePins.insert(45);
    anWritePins.insert(46);
    */

    wxBoxSizer* vboxsz = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* sizer = new wxFlexGridSizer( m_cols.GetCount(), 0, 0 );
    m_pins = new GridLineMeta[ wxGetApp().m_pincnt ];


	for ( int i=0; i < wxGetApp().m_pincnt; i++ )
    {
        m_pins[i].pin = i;

        for (int j=0; j < sizer->GetCols(); j++)
        {
            if ( m_cols[j].IsSameAs("pin") )
            {
                wxStaticText* text = new wxStaticText (this,wxID_ANY,"pin" + wxString::Format("%i",i));
                text->SetClientData((void*)&m_pins[i]);
                m_pins[i].pobjs.Add((wxObject*)text);
                sizer->Add(text);
            }
            else if (  m_cols[j].IsSameAs("INPUT") || m_cols[j].IsSameAs("INPUT_PULLUP")
                    || m_cols[j].IsSameAs("OUTPUT")
                    || m_cols[j].IsSameAs("digitalWrite") || m_cols[j].IsSameAs("digitalRead")
                    || m_cols[j].IsSameAs("LOW") || m_cols[j].IsSameAs("HIGH"))
            {
                wxToggleButton* tbutton = new wxToggleButton (this,wxID_ANY,m_cols[j]);
                tbutton->SetClientData((void*)&m_pins[i]);
                m_pins[i].pobjs.Add((wxObject*)tbutton);
                sizer->Add(tbutton);

                if ( !(m_cols[j].IsSameAs("INPUT") || m_cols[j].IsSameAs("INPUT_PULLUP") || m_cols[j].IsSameAs("OUTPUT")) )
                    tbutton->Disable();
            }
            else if ( m_cols[j].IsSameAs("analogWrite") )
            {
                if ( IsAnalogWriteable(i) )
                {
                    wxToggleButton* tbutton = new wxToggleButton (this,wxID_ANY,m_cols[j]);
                    tbutton->SetClientData((void*)&m_pins[i]);
                    m_pins[i].pobjs.Add((wxObject*)tbutton);
                    sizer->Add(tbutton);
                    tbutton->Disable();
                }
                else { sizer->Add(new wxWindow()); m_pins[i].pobjs.Add((wxObject*)NULL); }
            }
            else if ( m_cols[j].IsSameAs("analogWriteValue") )
            {
                if ( IsAnalogWriteable(i) )
                {
                    wxSlider* slider = new wxSlider (this,wxID_ANY,50, 0, 255,wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS);
                    slider->SetValue(0);
                    slider->SetLineSize(255);
                    slider->SetClientData((void*)&m_pins[i]);
                    m_pins[i].pobjs.Add((wxObject*)slider);

                    slider->Bind(wxEVT_SLIDER, &xPanel::OnScroll, this);
                    sizer->Add(slider);
                    slider->Disable();
                }
                else { sizer->Add(new wxWindow()); m_pins[i].pobjs.Add((wxObject*)NULL); }
            }
            else if ( m_cols[j].IsSameAs("analogRead") )
            {
                if ( IsAnalogReadable(i) )
                {
                    wxToggleButton* tbutton = new wxToggleButton (this,wxID_ANY,m_cols[j]);
                    tbutton->SetClientData((void*)&m_pins[i]);
                    m_pins[i].pobjs.Add((wxObject*)tbutton);
                    sizer->Add(tbutton);
                    tbutton->Disable();
                }
                else { sizer->Add(new wxWindow()); m_pins[i].pobjs.Add((wxObject*)NULL); }

            }
            else if ( m_cols[j].IsSameAs("ReadValue") || m_cols[j].IsSameAs("alias"))
            {
                //wxTextCtrl* tctl = new wxTextCtrl (this,wxID_ANY,m_cols[j]);
                wxStaticText* tctl = new wxStaticText (this,wxID_ANY,m_cols[j]);
                tctl->SetClientData((void*)&m_pins[i]);
                m_pins[i].pobjs.Add((wxObject*)tctl);
                sizer->Add(tctl);
                if (m_cols[j].IsSameAs("ReadValue"))
                    tctl->Disable();
            }
            else if ( m_cols[j].IsSameAs("external") )
            {
                awxLed* awx = new awxLed( this, wxID_ANY );
                awx->SetColour( awxLED_YELLOW );
                awx->SetState( awxLED_OFF );
                awx->SetClientData((void*)&m_pins[i]);
                m_pins[i].pobjs.Add((wxObject*)awx);
                sizer->Add(awx,1,wxALL|wxEXPAND,5);

                //wxTextCtrl* tctl = new wxTextCtrl (this,wxID_ANY,m_cols[j]);
                //wxStaticText* tctl = new wxStaticText (this,wxID_ANY,m_cols[j]);
                /*
                wxCheckBox* cbox = new wxCheckBox(this,wxID_ANY,wxEmptyString);
                cbox->SetClientData((void*)&m_pins[i]);
                m_pins[i].pobjs.Add((wxObject*)cbox);
                sizer->Add(cbox, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL);
                cbox->Disable();
                */
            }
            else
            {
                sizer->Add(new wxWindow());
            }
            //tb->Bind(wxEVT_TOGGLEBUTTON, &xPanel::OnClickToggleButton, this);
            //tb->Bind(wxEVT_TOGGLEBUTTON, &OnClickToggleButton, wxID_ANY, wxID_ANY, (wxObject*)glm);
            //sizer->Add(tb);
        }

        Bind( wxEVT_TOGGLEBUTTON, &xPanel::OnClickToggleButton, this );
        Bind( wxEVT_BUTTON, &xPanel::OnClick, this );
        //Bind( wxEVT_SLIDER, &xPanel::OnScroll, this );
    }

	m_tctl = new wxTextCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize, wxTE_MULTILINE | wxTE_RICH);

	//HideRow(0);
	//HideRow(1);

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


