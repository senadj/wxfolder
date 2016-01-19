#include <wx/app.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/bitz.h>

class xPanel : public wxPanel
{
public:

     wxBitzBitmap bmp;

     xPanel(wxWindow* parent) : wxPanel(parent)
     {
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxHORIZONTAL );
        wxBoxSizer* bSizer1 = new wxBoxSizer( wxVERTICAL );

        // NEW BMP
        bmp.m_vbrush.push_back(std::make_pair( wxWHITE_BRUSH, wxBLACK_BRUSH ));
        bmp.m_vbrush.push_back(std::make_pair( wxYELLOW_BRUSH, wxGREEN_BRUSH ));
        bmp.m_vbrush.push_back(std::make_pair( wxWHITE_BRUSH, wxRED_BRUSH ));

        bmp.m_vpen.push_back(std::make_pair( wxGREY_PEN, wxBLACK_PEN ));
        bmp.m_vpen.push_back(std::make_pair( wxGREY_PEN, wxGREEN_PEN ));
        bmp.m_vpen.push_back(std::make_pair( wxGREY_PEN, wxRED_PEN ));

        bmp.Create( wxVERTICAL, 3, 6 );

        for (int i=0;i<70;i++)
        {
            std::vector<unsigned char> v(32,0);
            v[i%32] = 0xff;
            wxBitz* bitz = new wxBitz(this, "oooooooo------------------------", &bmp, v);
            bitz->SetBrushPenCombo(i%3);
            bSizer2->Add( bitz , 0, 0/*wxEXPAND*/, 0 );
        }

        bSizer2->Add( bSizer1, 1, wxEXPAND, 0 );
        this->SetSizer( bSizer2 );
        this->Layout();
        //bSizer2->Fit( this );
        //bSizer2->Fit( parent->GetParent() );
     }
};
#include <wx/stattext.h>

class yPanel : public wxPanel
{
public:

     wxBitzBitmap bmp59;

     yPanel(wxWindow* parent) : wxPanel(parent)
     {
        wxBoxSizer* bSizer1 = new wxBoxSizer(wxHORIZONTAL );
        wxBoxSizer* bSizer2 = new wxBoxSizer( wxVERTICAL );

        bmp59.Create( wxHORIZONTAL, 5, 9 );

        for (int i=0;i<30;i++)
        {
            wxBitz* bitz = new wxBitz(this, "oooo", &bmp59, std::vector<unsigned char> (32,10+i));
            bSizer2->Add( bitz , 0, 0/*wxEXPAND*/, 0 );
        }

wxStaticText* text = new wxStaticText(this, wxID_ANY,"");
text->SetLabelMarkup("<span foreground='red'>breakfast</span> "
                     "available <big>HERE</big>");

        bSizer2->Add( text , 0, 0/*wxEXPAND*/, 0 );

        bSizer2->Add( bSizer1, 1, wxEXPAND, 0 );
        this->SetSizer( bSizer2 );
        this->Layout();
        //bSizer2->Fit( parent->GetParent() );
     }
};
#include <wx/scrolwin.h>
class uPanel : public wxScrolledCanvas

{
public:

     wxBitzBitmap bmp59;

     uPanel(wxWindow* parent) : wxScrolledCanvas
(parent) //,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxVSCROLL)
     {

        wxBoxSizer* bSizer1 = new wxBoxSizer( wxHORIZONTAL );
        wxBoxSizer* bSizer2 = new wxBoxSizer( wxVERTICAL );
        //wxBoxSizer* bSizer3 = new wxBoxSizer( wxVERTICAL );

        wxFlexGridSizer* fxs = new wxFlexGridSizer(3,3,3);

        bmp59.Create( wxHORIZONTAL, 5, 9 );

        for (int i=0;i<30;i++)
        {
            wxBitz* bitz = new wxBitz(this, "oooo", &bmp59, std::vector<unsigned char> (32,10+i));
            fxs->Add( bitz , 0, 0/*wxEXPAND*/, 0 );
            wxStaticText* hext = new wxStaticText(this, wxID_ANY,"0123456789abcdef");
            fxs->Add( hext , 0, 0/*wxEXPAND*/, 0 );
            wxStaticText* dext = new wxStaticText(this, wxID_ANY,wxString::Format("%i",i*i*i*i*i*i));
            fxs->Add( dext , 0, /*wxEXPAND*/wxALIGN_RIGHT, 0 );
        }

        for (int j=0; j<10; j++)
        {
            wxButton* btn = new wxButton(this, wxID_ANY,wxString::Format("%i",j*j*j));
            bSizer2->Add( btn , 0, 0/*wxEXPAND*/, 0 );
        }


        bSizer1->Add( fxs, 1, wxEXPAND, 0 );
        bSizer1->Add( bSizer2, 1, wxEXPAND, 0 );
this->SetScrollbars(20, 20, 50, 50);
this->ShowScrollbars( wxSHOW_SB_NEVER,  wxSHOW_SB_ALWAYS);

        this->SetSizer( bSizer1 );
        this->Layout();
        //bSizer2->Fit( parent->GetParent() );
     }
};



class zPanel : public wxPanel
{
public:

     wxBitzBitmap bmp25;

     zPanel(wxWindow* parent) : wxPanel(parent)
     {
        bmp25.Create( wxVERTICAL, 2, 5 );
        wxBitzBitmapWindow* bmpwin = new wxBitzBitmapWindow(this,&bmp25);
     }
};


class xFrame : public wxFrame
{
public:
    xFrame() : wxFrame (NULL, wxID_ANY, "wxBitz Demo", wxDefaultPosition, wxSize(900,300))
    {
        wxNotebook* nb = new wxNotebook(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxNB_TOP|wxNB_NOPAGETHEME);

        nb->AddPage( new uPanel(nb),"upanel" );//,false,0);
        nb->AddPage( new xPanel(nb),"xpanel" );//,true,0);
        nb->AddPage( new yPanel(nb),"ypanel" );//,false,1);
        nb->AddPage( new zPanel(nb),"zpanel" );//,false,0);
    }
};


class xApp : public wxApp
{
    virtual bool OnInit ()
    {
        xFrame * frame = new xFrame();
        frame ->Center();
        frame ->Show();
        return true ;
    }
};

IMPLEMENT_APP(xApp )

