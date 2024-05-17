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



/** Plot layer implementing a y-scale ruler.
 If align is set to mpALIGN_CENTER, the ruler is fixed at X=0 in the coordinate system.
 If the align is set to mpALIGN_TOP or mpALIGN_BOTTOM, the axis is always drawn respectively at
 top or bottom of the window. A label is plotted at the top-right hand of the ruler.
 The scale numbering automatically adjusts to view and zoom factor.
 */
class WXDLLIMPEXP_MATHPLOT mpScaleY2: public mpScaleY
{
  public:
    /** Full constructor.
     @param name Label to plot by the ruler
     @param flags Set the position of the scale with respect to the window.
     @param grids Show grid or not. Give false (default) for not drawing the grid. */
    mpScaleY2(const wxString &name = _T("Y"), int flags = mpALIGN_CENTERY, bool grids = false) :
        mpScaleY(name, flags, grids)
    {
      ;
    }

    /** Layer plot handler.
     This implementation will plot the ruler adjusted to the visible area. */
    virtual void DoPlot(wxDC &dc, mpWindow &w);

  protected:

  DECLARE_DYNAMIC_CLASS(mpScaleY2)
};

// Minimum axis label separation
#define MIN_X_AXIS_LABEL_SEPARATION 64
#define MIN_Y_AXIS_LABEL_SEPARATION 32
//-----------------------------------------------------------------------------
// mpScaleY2
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(mpScaleY2, mpScaleY)

void mpScaleY2::DoPlot(wxDC &dc, mpWindow &w)
{
  int orgx = GetOrigin(w);

  // Draw nothing if we are outside margins
  if (!m_drawOutsideMargins && ((orgx > (w.GetScreenX() - w.GetMarginRight())) || (orgx + 1 < w.GetMarginLeft())))
    return;

  // Draw Y axis
  dc.DrawLine(orgx + 1, m_plotBondaries.startPy, orgx + 1, m_plotBondaries.endPy);

  const double scaleY = w.GetScaleY();
  const double step = GetStep(scaleY);
  const double end = w.GetPosY() + (double)w.GetScreenY() / scaleY;

  wxString fmt;
  if (m_labelFormat.IsEmpty())
  {
    double maxScaleAbs = fabs(w.GetDesiredYmax());
    double minScaleAbs = fabs(w.GetDesiredYmin());
    double endscale = (maxScaleAbs > minScaleAbs) ? maxScaleAbs : minScaleAbs;
    if ((endscale < 1e4) && (endscale > 1e-3))
      fmt = _T("%.2f");
    else
      fmt = _T("%.2e");
  }
  else
  {
    fmt = m_labelFormat;
  }

  double n = floor((w.GetPosY() - (double)(w.GetScreenY()) / scaleY) / step) * step;

  wxCoord tmp = 65536;
  wxCoord labelW = 0;
  // Before staring cycle, calculate label height
  wxCoord labelHeigth = 0;
  wxString s;
  wxCoord tx = 0, ty = 0;
  s.Printf(fmt, n);
  dc.GetTextExtent(s, &tx, &labelHeigth);
  labelHeigth /= 2;

  // Draw grid, ticks and label
  for (; n < end; n += step)
  {
    // To have a real zero
    if (fabs(n) < 1e-10)
      n = 0;
    const int p = (int)((w.GetPosY() - n) * scaleY);
    if ((p > m_plotBondaries.startPy + labelHeigth) && (p < m_plotBondaries.endPy - labelHeigth))
    {
      // Draw axis grids
      if (m_grids && (n != 0))
      {
        dc.SetPen(m_gridpen);
        dc.DrawLine(m_plotBondaries.startPx + 1, p, m_plotBondaries.endPx - 1, p);
      }

      // Draw axis ticks
      if (m_ticks)
      {
        dc.SetPen(m_pen);
        if (m_flags == mpALIGN_BORDER_LEFT)
        {
          dc.DrawLine(orgx, p, orgx + 4, p);
        }
        else
        {
          dc.DrawLine(orgx - 4, p, orgx, p);
        }
      }

      if (IsLogAxis())
      {
        s = FormatLogValue(n);
        if (s.IsEmpty())
          continue;
      }
      else
        s.Printf(fmt, n*2);

      // Print ticks labels
      dc.GetTextExtent(s, &tx, &ty);
#ifdef MATHPLOT_DO_LOGGING
      if (ty != labelHeigth)
        wxLogMessage(_T("mpScaleY::Plot: ty(%d) and labelHeigth(%d) differ!"), ty, labelHeigth);
#endif
      labelW = (labelW <= tx) ? tx : labelW;
      if ((tmp - p + labelHeigth) > MIN_Y_AXIS_LABEL_SEPARATION)
      {
        if ((m_flags == mpALIGN_BORDER_LEFT) || (m_flags == mpALIGN_RIGHT))
          dc.DrawText(s, orgx + 4, p - ty / 2);
        else
          dc.DrawText(s, orgx - tx - 4, p - ty / 2);
        tmp = p - labelHeigth;
      }
    }
  }

  // Draw axis name
  DrawScaleName(dc, w, orgx, labelW);
}




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
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame) EVT_MENU(Demo_Quit, MyFrame::OnQuit)
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
}

void MyFrame::CreatePlot(void)
{
	m_plot = new mpWindow(this);
	m_plot->EnableDoubleBuffer(true);
	m_plot->SetMargins(50, 50, 50, 50);

	mpScaleX *bottomAxis = new mpScaleX(wxT("X"), mpALIGN_CENTERX, true, mpX_NORMAL);
	bottomAxis->SetLabelFormat("%g");
	mpScaleY *leftAxis = new mpScaleY(wxT("Y"), mpALIGN_LEFT, true);
	leftAxis->SetLabelFormat("%g");

	// add a second Y axis on the right margin
	mpScaleY *rightAxis = new mpScaleY2(wxT("Y2"), mpALIGN_RIGHT, true);
	leftAxis->SetLabelFormat("%g");

	wxFont graphFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxPen axispen(*wxRED, 2, wxPENSTYLE_SOLID);
	bottomAxis->SetFont(graphFont);
	leftAxis->SetFont(graphFont);
	bottomAxis->SetPen(axispen);
	leftAxis->SetPen(axispen);

	m_plot->AddLayer(bottomAxis);
	m_plot->AddLayer(leftAxis);
	m_plot->AddLayer(rightAxis);


    //mpScaleY *rightAxis = new mpScaleY(wxT("Y2"), mpALIGN_RIGHT, true, true);
    //m_plot->AddLayer(rightAxis);

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
		serie->AddData(i / 10.0 + 40000, sin(i / 10.0), true);
	m_plot->Fit();  //  UpdateAll
	legend->SetNeedUpdate();
	serie->SetLimitPercent(0);

	mpFXYVector *serie2 = m_plot->GetXYSeries(1);
    for (int i = 0; i < 100; i++)
		serie2->AddData(i / 10.0 + 40000 + 5, 3 * sin(i / 10.0), true);
    //serie2->SetY2Axis(true);
    serie2->SetBrush(*wxRED);
	serie2->SetSymbol(mpsCircle);
	serie2->SetLimitPercent(0);

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
