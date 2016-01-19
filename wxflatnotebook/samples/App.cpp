#include "App.h"
#include <wx/image.h> 
#include <wx/xrc/xmlres.h>
#include "Frame.h"
#include <wx/sysopt.h>
 
IMPLEMENT_APP(App)

Frame *pMainFrame = (Frame *) NULL;


// Externs
extern void InitXmlResource();

App::App(void)
{
}

App::~App(void)
{
}

// Initialize our application
bool App::OnInit(void)
{ 
	// Init resources and add the PNG handler
	wxSystemOptions::SetOption(wxT("msw.remap"), 0); 
	wxXmlResource::Get()->InitAllHandlers(); 
 	wxImage::AddHandler( new wxPNGHandler ); 
	wxImage::AddHandler( new wxCURHandler ); 
	wxImage::AddHandler( new wxBMPHandler );
	wxImage::AddHandler( new wxXPMHandler );

	InitXmlResource();

	pMainFrame = new Frame(NULL, wxT("Frame"));
	 

	// show the main frame
	pMainFrame->Show(TRUE);

	// set the main frame to be the main window
	SetTopWindow(pMainFrame);
	return TRUE;	
}

int App::OnExit(void) 
{

	return 0;
}
