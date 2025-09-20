// g++ -w `wx-config --cxxflags` -I. -c main.cpp &&  g++ main.o `wx-config --libs core,plotctrl`
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/timer.h>
#include <wx/plotctrl/plotctrl.h>
#include <wx/scrolbar.h>

class xPlotCtrl : public wxPlotCtrl
{
public:

    xPlotCtrl(wxWindow* parent) : wxPlotCtrl(parent)
    {
        //SetShowXAxis(false);
        //SetDrawGrid(false);
        SetCorrectTicks(false); // smooth scrooling
        SetDrawLines(true); // draw interconnecting lines
        SetDrawSymbols(false); // do not draw square points
        /*m_xAxisScrollbar->SetScrollbar(0,0,0,0,false);
        m_yAxisScrollbar->SetScrollbar(0,0,0,0,false);
        m_xAxisScrollbar->Hide();
        m_yAxisScrollbar->Hide();*/

    }

    // set X axis tick labels HH:MM:SS
    void CalcXAxisTickPositions()
    {
        double current = ceil(m_viewRect.GetLeft() / m_xAxisTick_step) * m_xAxisTick_step;
        m_xAxisTicks.Clear();
        m_xAxisTickLabels.Clear();
        int i, x, windowWidth = GetPlotAreaRect().width;
        for (i = 0; i < m_xAxisTick_count; i++)
        {
            if (!IsFinite(current, wxT("axis label is not finite"))) { wxLogMessage("finifini");return;}
            x = GetClientCoordFromPlotX( current );

            if ((x >= -1) && (x < windowWidth+2))
            {
                m_xAxisTicks.Add(x);
                m_xAxisTickLabels.Add(wxDateTime(wxLongLong(current)).FormatISOTime());
            }

            current += m_xAxisTick_step;
        }
    }

    void AddDataCurveFromFunction(const wxString& fstr)
    {
        wxPlotFunction* func = new wxPlotFunction(fstr);
        wxPlotData* data = new wxPlotData(*func, wxGetUTCTimeMillis().ToDouble() /*start*/, 100/*dx*/, 10000/*points*/);
        func->Destroy() /*unref data*/;
        wxDELETE(func);
        data->SetFilename(fstr.Mid(1,5));
        AddCurve(data,false);
    }
};

class xFrame : public wxFrame
{
public:

    wxTimer     m_timer;
    xPlotCtrl*  m_plot;
    double      m_range;

    xFrame() : wxFrame (nullptr, wxID_ANY, "plotmini", wxDefaultPosition, wxSize(800,500)),m_range(16000)
    {
        //CreateStatusBar(4);
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* hsizer = new wxBoxSizer(wxHORIZONTAL);

        m_plot = new xPlotCtrl(this);

        std::vector<wxButton*> vbuttons;
        for ( int i=0; i<11; i++)
        {
            vbuttons.push_back(new wxButton(this,wxID_ANY,"B" + wxString::Format("%i",(i+1)*(i+1)*2000)));
            vbuttons.back()->Bind(wxEVT_BUTTON, [=](wxCommandEvent&) {  m_range = (i+1)*(i+1)*2000;
            /*m_plot->SetShowXAxis((i==0)?false:true);*/
            if (i==10)
                {}
            });
            hsizer->Add(vbuttons.back());
        }


        m_plot->SetInitialSize(wxSize(1024,400));
        m_plot->AddDataCurveFromFunction("(1+sin(x*3.14/7000))*512");
        m_plot->AddDataCurveFromFunction("(3.7+sin(x*3.14/1800))*128");
        m_plot->AddDataCurveFromFunction("(2.7+sin(x*3.14/2200))*96");
        m_plot->AddDataCurveFromFunction("(2.4+sin(x*3.14/2500))*222");

        sizer->Add(hsizer,wxSizerFlags().Expand()/*.Proportion(6)*/);
        sizer->Add(m_plot, wxSizerFlags().Expand().Proportion(1));
        //SetStatusText();
        SetSizerAndFit(sizer);

        this->Bind(wxEVT_TIMER, &xFrame::OnTimer, this);
        m_timer.SetOwner(this);
        m_timer.Start(100);

        m_plot->SetViewRect(wxRect2DDouble( wxGetUTCTimeMillis().ToDouble(), -100, m_range /*control win scope*/,1200));
    }

    void OnTimer(wxTimerEvent&)
    {
        //m_plot-SetScrollPos(m_plot->)
        m_plot->SetViewRect(wxRect2DDouble( wxGetUTCTimeMillis().ToDouble(), -100, m_range /*control win scope*/,1200));
    }
};


class xApp:public wxApp{bool OnInit(){auto* f=new xFrame;f->Center();f->Show(true);return true;}};IMPLEMENT_APP(xApp)

