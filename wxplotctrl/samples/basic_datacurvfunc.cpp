// g++ -w `wx-config --cxxflags` -I. -c main.cpp &&  g++ main.o `wx-config --libs core,plotctrl`
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/plotctrl/plotctrl.h>

class xPlotFunction : public wxPlotFunction
{
public:
    xPlotFunction() : wxPlotFunction() { Create("x*x-0.7"); }
    //bool Ok() const wxOVERRIDE { return true; }
} myfunc;

class xPlotCurve : public wxPlotCurve
{
public:
    xPlotCurve() : wxPlotCurve() { SetRefData(new wxPlotCurveRefData()); }
    double GetY( double x ) const { return -x*x+3000; }
} mycurv;


class xFrame : public wxFrame
{
public:
    xFrame() : wxFrame ( nullptr, wxID_ANY, "Plot Function and Curve", wxDefaultPosition, wxSize(800,500) )
    {
        wxPlotCtrl* plot = new wxPlotCtrl(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxPLOTCTRL_DEFAULT);

		///////////////////////////////////////////////////////////////
        // wxPlotData
        wxPlotData pdata;
        pdata.Create(100);
        double* xdata = pdata.GetXData();
        double* ydata = pdata.GetYData();

        for (double i=0; i< 100; i++)
        {
            *xdata++ = i-50;
            *ydata++ = i*i/3;
        }

        pdata.CalcBoundingRect();
        plot->AddCurve(pdata,true,true);
		///////////////////////////////////////////////////////////////
        // wxPlotFunction
        wxPlotFunction pfunc;
        pfunc.Create("x*x/6+200");
        plot->AddCurve(pfunc,true,true);
		///////////////////////////////////////////////////////////////
        // derived classes
        plot->AddCurve(myfunc, true, true);
        plot->AddCurve(&mycurv, true, true);
    }
};

class xApp:public wxApp{bool OnInit(){auto* f=new xFrame;f->Center();f->Show(true);return true;}};IMPLEMENT_APP(xApp)

