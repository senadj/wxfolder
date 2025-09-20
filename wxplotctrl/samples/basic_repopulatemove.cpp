// g++ -w `wx-config --cxxflags` -I. -c main.cpp &&  g++ main.o `wx-config --libs core,plotctrl`
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/plotctrl/plotctrl.h>

class xPlotData : public wxPlotData
{
public:

    double m_xoffset; // plot x origin

    xPlotData() : wxPlotData(), m_xoffset(-10)
    {
        Create(100,false/* all zeros */);
        RePopulate();
    }

    void RePopulate()
    {
        double* xdata = this->GetXData();
        double* ydata = this->GetYData();

        for (double i=0; i < 100; i++)
        {
            *xdata++ = i+m_xoffset;
            *ydata++ = (i+m_xoffset)*(i+m_xoffset);
        }
        m_xoffset++;
        CalcBoundingRect();
    }
};

class xFrame : public wxFrame
{
public:

    wxPlotCtrl* m_plot;
    wxTimer     m_timer;
    xPlotData   m_data;

    xFrame() : wxFrame (nullptr, wxID_ANY,"plotmini",wxDefaultPosition,wxSize(800,500))
    {
        m_plot = new wxPlotCtrl(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxPLOTCTRL_DEFAULT);
        m_plot->AddCurve(m_data,true,true);

        this->Bind(wxEVT_TIMER, &xFrame::OnTimer, this);
        m_timer.SetOwner(this);
        m_timer.Start(100);
    }

    void OnTimer( wxTimerEvent& event )
    {
       m_data.RePopulate();
       wxRect2DDouble brect = m_data.GetBoundingRect(); // m_plot->GetDataCurve(m_plot->GetCurveCount()-1)->GetBoundingRect();
       m_plot->SetOrigin(brect.m_x,brect.m_y); // calls: m_plot->Redraw(...);
    }
};


class xApp:public wxApp{bool OnInit(){auto* f=new xFrame;f->Center();f->Show();return true;}};IMPLEMENT_APP(xApp)
