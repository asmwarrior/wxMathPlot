/***************************************************************
 * Name:      MathPlotConfigDemo.cpp
 * Purpose:   Code for Application Class
 * Author:    Lionel ()
 * Created:   2021-01-24
 * Copyright: Lionel ()
 * License:
 **************************************************************/

// ============================================================================
// declarations
// ============================================================================
// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

#include "mathplot.h"

#include <wx/ffile.h>

// Define a new application type, each program should derive a class from wxApp
class MyApp: public wxApp
{
	public:
		// override base class virtuals
		// ----------------------------

		// this one is called on application startup and is a good place for the app
		// initialization (doing it here and not in the ctor allows to have an error
		// return: if OnInit() returns false, the application terminates)
		virtual bool OnInit() wxOVERRIDE;
};

// Define a new frame type: this is going to be our main frame
class MyFrame: public wxFrame
{
	public:
		// ctor(s)
		MyFrame(const wxString &title);

		void CreatePlot(void);

		// event handlers (these functions should _not_ be virtual)
		void OnQuit(wxCommandEvent &event);
		void OnAbout(wxCommandEvent &event);

	private:
		mpWindow *m_plot = NULL;

		// any class wishing to process wxWidgets events must use this macro
	wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
	// menu items
	Demo_Quit = wxID_EXIT,

	// it is important for the id corresponding to the "About" command to have
	// this standard value as otherwise it won't be handled properly under Mac
	// (where it is special and put into the "Apple" menu)
	Demo_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(Demo_Quit, MyFrame::OnQuit)
EVT_MENU(Demo_About, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
	// call the base class initialization method, currently it only parses a
	// few common command-line options but it could be do more in the future
	if (!wxApp::OnInit())
		return false;

	// create the main application window
	MyFrame *frame = new MyFrame("Demo wxWidgets MathPlot App with config dialog");

	// and show it (the frames, unlike simple controls, are not shown when
	// created initially)
	frame->Show(true);

	// success: wxApp::OnRun() will be called which will enter the main message
	// loop and the application will run. If we returned false here, the
	// application would exit immediately.
	return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

//#undef wxUSE_STATUSBAR

// frame constructor
MyFrame::MyFrame(const wxString &title) :
		wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800,600))
{
	// set the frame icon
	SetIcon(wxICON(sample));

	// create a menu bar
	wxMenu *fileMenu = new wxMenu;

	// the "About" item should be in the help menu
	wxMenu *helpMenu = new wxMenu;
	helpMenu->Append(Demo_About, "&About\tF1", "Show about dialog");

	fileMenu->Append(Demo_Quit, "E&xit\tAlt-X", "Quit this program");

	// now append the freshly created menu to the menu bar...
	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, "&File");
	menuBar->Append(helpMenu, "&Help");

	// ... and attach this menu bar to the frame
	SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to MathPlot wxWidgets!");
#endif // wxUSE_STATUSBAR

	// Create the plot window
	CreatePlot();

	// Add plot to the frame
	wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_plot, 1, wxALL | wxEXPAND, 5);
	SetSizer(sizer);

	wxLog* logger = new wxLogStream(&std::cout);
    wxLog::SetActiveTarget(logger);

    FILE *logFile = fopen("log.txt", "w");
    wxLogChain* logToFile = new wxLogChain(new wxLogStderr(logFile));
    wxLog::SetActiveTarget(logToFile);


}

void MyFrame::CreatePlot(void)
{
	m_plot = new mpWindow(this);
	m_plot->EnableDoubleBuffer(true);
	m_plot->SetMargins(50, 20, 50, 50);

	mpScaleX *bottomAxis = new mpScaleX(wxT("X"), mpALIGN_CENTERX, true, mpX_NORMAL);
	bottomAxis->SetLabelFormat("%g");
	mpScaleY *leftAxis = new mpScaleY(wxT("Y"), mpALIGN_CENTERY, true);
	leftAxis->SetLabelFormat("%g");

	wxFont graphFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxPen axispen(*wxRED, 2, wxPENSTYLE_SOLID);
	bottomAxis->SetFont(graphFont);
	leftAxis->SetFont(graphFont);
	bottomAxis->SetPen(axispen);
	leftAxis->SetPen(axispen);

	m_plot->AddLayer(bottomAxis);
	m_plot->AddLayer(leftAxis);
	mpTitle *plotTitle;
	m_plot->AddLayer(plotTitle = new mpTitle(wxT("Demo MathPlot")));

	wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	plotTitle->SetFont(titleFont);

	mpInfoCoords *info;
	m_plot->AddLayer(info = new mpInfoCoords());
	info->SetVisible(true);

	mpInfoLegend *legend;
	m_plot->AddLayer(legend = new mpInfoLegend());
	legend->SetItemDirection(mpHorizontal);
	legend->SetVisible(true);

	m_plot->Fit();

	// add a simple sinus serie
	mpFXYVector *serie = m_plot->GetXYSeries(0);
	for (int i = 0; i < 100; i++)
		serie->AddData(i / 10.0, sin(i / 10.0), true);
	m_plot->Fit();  //  UpdateAll
	legend->SetNeedUpdate();

	// Some decoration
	serie->SetBrush(*wxYELLOW);
	serie->SetSymbol(mpsCircle);
}

// event handlers

void MyFrame::OnQuit(wxCommandEvent&WXUNUSED(event))
{
	// true is to force the frame to close
	Close(true);
}

void MyFrame::OnAbout(wxCommandEvent&WXUNUSED(event))
{
	wxMessageBox(wxString::Format("Welcome to %s!\n"
			"\n"
			"This is the minimal wxWidgets sample\n"
			"running under %s.",
	wxVERSION_STRING, wxGetOsDescription()), "About wxWidgets minimal sample",
	wxOK | wxICON_INFORMATION, this);
}
