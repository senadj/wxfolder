// g++ -w `wx-config --cxxflags` -I. -c main.cpp &&  g++ main.o `wx-config --libs core,plotctrl`
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/timer.h>
#include <wx/plotctrl/plotcirc.h>
#include <wx/plotctrl/plotctrl.h>
#include <wx/scrolbar.h>

static double mx;
static double mxf;

typedef wxPlotCircularBuffer<0,1024> xPlotData1024;

class xPlotCtrl : public wxPlotCtrl
{
public:

    // set X axis tick labels HH:MM:SS
    void CalcXAxisTickPositions() wxOVERRIDE
    {
        double current = ceil(m_viewRect.GetLeft() / m_xAxisTick_step) * m_xAxisTick_step;
        m_xAxisTicks.Clear();
        m_xAxisTickLabels.Clear();
        int i, x, windowWidth = GetPlotAreaRect().width;
        for (i = 0; i < m_xAxisTick_count; i++)
        {
            if (!IsFinite(current, "axis label is not finite")) { wxLogMessage("finifini");return;}

            x = GetClientCoordFromPlotX( current );

            if ((x >= -1) && (x < windowWidth+2))
            {
                m_xAxisTicks.Add(x);
                m_xAxisTickLabels.Add(wxDateTime(wxLongLong(current)).FormatISOTime());
                //m_xAxisTickLabels.Add(wxString::Format("%d",x));
            }
            //else {wxLogMessage("ajajaj");}


            current += m_xAxisTick_step;
        }
    }

    xPlotCtrl(wxWindow* parent) : wxPlotCtrl( parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxPLOTCTRL_DEFAULT )
    {
        this->SetCorrectTicks(false); // false = scroll smoothly
        //this->SetFitPlotOnNewCurve(false);
        this->SetDrawGrid(false);
        //this->SetDrawLines(false); // Draw the interconnecting straight lines between data points
        this->SetDrawSymbols(false); // Draw the data curve symbols on the plotctrl
        //m_plot->SetOrigin(50,0);
        this->SetShowKey(false); // default=true

        m_xAxisScrollbar->SetScrollbar(0,0,0,0,false);
        m_yAxisScrollbar->SetScrollbar(0,0,0,0,false);
        m_xAxisScrollbar->Hide();
        m_yAxisScrollbar->Hide();
    }

    void SetMyZoom(const wxRect2DDouble &view)
    {
        SetZoom(m_zoom.m_x, m_zoom.m_y/*reuse zoom*/, view.m_x, view.m_y, false/*send_event*/);
    };

    void RefreshPart()
    {
        wxRect arect = m_area->GetClientRect();
        arect.SetLeft(700);
        m_area->Refresh(false,&arect);
    }

    wxRect GetPlotAreaRect()
    {
        //wxRect2DDouble dbl( (mx-mxf>15000) ? mx-15000 : mxf, -10, 15100, viewY )
        //wxRect2DDouble dbl( mx-100, -10, 100, 1050 );
        return m_area->GetClientRect();
        //return m_area->GetScreenRect();
        //arect.GetLeftTop();
    
        //wxRect brect = wxRect()
        //m_area->Refresh(false,);
    }

    //~xPlotCtrl(){ wxDELETE(m_buff); }
};

#include <wx/textctrl.h>
#include <wx/button.h>

class xFrame : public wxFrame
{
public:
    xPlotCtrl* m_plot;
    xPlotData1024* m_buff;
    xPlotData1024* m_buff2;

    wxTimer     m_timer;
    ~xFrame(){  }
    xFrame() : wxFrame (nullptr, wxID_ANY,"plotmini",wxDefaultPosition,wxDefaultSize)
    {
        CreateStatusBar(4);
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* hsizer = new wxBoxSizer(wxHORIZONTAL);

        std::vector<wxButton*> vbuttons;
        for ( int i=0; i<9; i++)
        {
            vbuttons.push_back(new wxButton(this,wxID_ANY,wxString::Format("Button%d",i)));
            vbuttons.back()->Bind(wxEVT_BUTTON, [=](wxCommandEvent&) { wxLogMessage(wxString::Format("B%d",i)); });
            hsizer->Add(vbuttons.back());
        }

        sizer->Add(hsizer,wxSizerFlags().Expand()/*.Proportion(6)*/);
        mxf = wxGetUTCTimeMillis().ToDouble();// - 1587989996000;

        wxTextCtrl* m_tctl = new wxTextCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize, wxTE_MULTILINE | wxTE_RICH);
        delete wxLog::SetActiveTarget(new wxLogTextCtrl(m_tctl));
        m_tctl->SetMinSize(wxSize(sizer->GetSize().GetWidth(),120));
        sizer->Add(m_tctl,wxSizerFlags().Expand()/*.Proportion(6)*/);

        m_plot = new xPlotCtrl(this);
        //m_plot->SetViewRect( wxRect2DDouble(mxf-50,-2,300,1050) );
        //m_plot->SetDefaultBoundingRect(wxRect2DDouble(mxf-50,-2,300,1050)); // used when no curves
        m_plot->SetInitialSize(wxSize(800,400));
        sizer->Add(m_plot, wxSizerFlags().Expand().Proportion(1));

        m_buff = new xPlotData1024(2500);
        m_buff->SetFilename("A0");
        if ( ! m_plot->AddCurve(m_buff,/*select*/false,/*send_event*/false) ) wxLogMessage("NOT AddCurve");

        m_buff2 = new xPlotData1024(2500);
        m_buff2->SetFilename("A1");
        if ( ! m_plot->AddCurve(m_buff2,/*select*/false,/*send_event*/false) ) wxLogMessage("NOT AddCurve");

        this->Bind(wxEVT_TIMER, &xFrame::OnTimer, this);
        m_timer.SetOwner(this);
        m_timer.Start(20);

        this->SetStatusText(wxString::FromDouble(mxf,0),3);
        this->SetSizerAndFit(sizer);
        //sizer->SetSizeHints(this);
    }

    void OnTimer( wxTimerEvent& event )
    {
        static bool baba = false;
        static int cnt = 0;

        mx = wxGetUTCTimeMillis().ToDouble();// - 1587989996000;
        wxDateTime(wxLongLong(mx)).FormatISOTime();

        if (cnt > 600 )
        {
        wxRect arect = m_plot->GetPlotAreaRect();
        //arect.gett
        SetStatusText(wxString::Format("width: %d",arect.GetWidth()));
        SetStatusText(wxString::Format("height: %d",arect.GetHeight()),1);
        SetStatusText(wxString::Format("left: %d",arect.GetLeft()),2);
        SetStatusText(wxString::Format("right: %d",arect.GetRight()),3);
        }
        else
        {
        SetStatusText(wxString::Format("%d",cnt++));
        SetStatusText(wxString::FromDouble(mx,0),1);
        SetStatusText(wxDateTime(wxLongLong(mx)).FormatISOTime(),2);
        SetStatusText(wxString::Format("zoomX: %.8f",m_plot->GetZoom().m_x),3);
        }

        //this->SetStatusText(wxString::Format("%d", m_plot->m_index1 - m_plot->m_begin1),1);
        //this->SetStatusText(wxString::Format("%u", static_cast<unsigned long>(m_plot->backx())),2);
        //this->SetStatusText(wxString::Format("%u", static_cast<unsigned long>(m_plot->backy())),3);

        if (m_buff->backy()>1023) baba=true;
        if (m_buff->backy()<0)    baba=false;

        if (baba) 
            m_buff->push_back( mx, m_buff->backy()-10 );
        else 
            m_buff->push_back( mx, m_buff->backy()+10 );

        m_buff2->push_back( mx, 400*cos(m_buff->backy()/1024) );

        double viewY = 1050;

        //m_plot->SetViewRect( wxRect2DDouble(mx-5000,-2,m_buff->widthx(),255) );
        //m_plot->SetViewRect( wxRect2DDouble(mx-5100,-10,52000,270) );
        //m_plot->SetViewRect( wxRect2DDouble( mx-5000, -10, 5100, 270 ) );
        if ( cnt > 600 )
        {
            m_buff->SetWindow(-1);
            //m_plot->SetViewRect( wxRect2DDouble( (mx-mxf>15000) ? mx-15000 : mxf, -10, 15100, viewY ) );
            m_plot->SetMyZoom( wxRect2DDouble( (mx-mxf>15000) ? mx-15000 : mxf, -10, 15100, viewY ) );
            //m_plot->RefreshPart();
        }
        else if ( cnt > 400 )
        {
            m_buff->SetWindow(200);
            m_plot->SetMyZoom( wxRect2DDouble( (mx-mxf>15000) ? mx-15000 : mxf, -10, 15100, viewY ) );
        }
        else if ( cnt > 200 )
            m_plot->SetViewRect( wxRect2DDouble( (mx-mxf>15000) ? mx-15000 : mxf, -10, 15100, viewY ) );
        else if ( cnt > 40 )
        {
            m_buff->SetWindow(30);
            m_plot->SetViewRect( wxRect2DDouble( (mx-mxf>5000) ? mx-5000 : mxf, -10, 5100, viewY ) );
        }
        else
            m_plot->SetViewRect( wxRect2DDouble( (mx-mxf>5000) ? mx-5000 : mxf, -10, 5100, viewY ) );


       //m_plot->SetOrigin(brect.m_x,brect.m_y); // calls: m_plot->Redraw(...);
       //m_plot->SetFixAspectRatio()Freeze();
       //m_plot->SetZoom(wxPoint2DDouble(0.7,180));
       //m_plot->SetOrigin(brect.m_x-10,-20); // calls: m_plot->Redraw(...);
       //data << (brect.m_x-10); GetStatusBar()->SetStatusText(data,2);
       //wxPoint2DDouble zpo = m_plot->GetZoom();
       //wxLogMessage(wxString(std::to_string(zpo.m_x)) + " " + wxString(std::to_string(zpo.m_y)));
       //m_plot->SetFitPlotOnNewCurve(true);
       //m_plot->SetZoom(1,2,brect.m_x,0);
       //m_plot->SetZoom(wxPoint2DDouble(1,0));
       //m_plot->SetZoom(wxRect(4,1,brect.m_x,-1
       //m_plot->SetDefaultBoundingRect(m_plot->GetDataCurve(m_plot->GetCurveCount()-1)->GetBoundingRect());
       //m_plot->SetDefaultBoundingRect(wxRect2DDouble( brect.m_x,-1.25,1250,2.5));
       //m_plot->SetDefaultBoundingRect(wxRect2DDouble(-2,-2,100,1));
       //m_plot->Redraw(wxPLOTCTRL_REDRAW_PLOT);
       
    }

};



class xApp:public wxApp{bool OnInit(){auto* f=new xFrame;f->Center();f->Show(true);return true;}};IMPLEMENT_APP(xApp)


class xChartTime
{
    wxLongLong m_time;

public:

    xChartTime() { update(); }
    double get() { return m_time.ToDouble(); }
    void update(){ m_time = wxGetUTCTimeMillis(); }
    double plus5s(){ return m_time.ToDouble() + 5000; }
    double minus5s(){ return m_time.ToDouble() - 5000; }
    double plus1s(){ return m_time.ToDouble() + 1000; }
    double minus1s(){ return m_time.ToDouble() - 1000; }
} ct;
