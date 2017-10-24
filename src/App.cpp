/*
 * App.cpp
 *
 */

#include "App.h"

#include "wx/wx.h"
#include "wx/msgout.h"
#include "wx/xrc/xmlres.h"
#include "wx/stdpaths.h"
#include "MainFrame.h"

bool App::OnInit()
{
  // In order to fix APPNAME to wxcid in WX_${APPNAME}_DATA_DIR env var.
  SetAppName("wxscid");

  // Make sure to call this first
  wxInitAllImageHandlers();
  wxXmlResource::Get()->InitAllHandlers();

  wxMessageOutputStderr err;
  err.Printf(wxT("Data dir: %s.\n"), getDataDir());

  // Load all of the XRC files that will be used.
  if (!wxXmlResource::Get()->LoadAllFiles(getDataDir() + "/xrc")) {
    return false;
  }

  frame = new MainFrame(NULL, wxID_ANY, _T("WxScid"), wxDefaultPosition, wxSize(800, 600));
  frame->Show(true);
  SetTopWindow(frame);

  return true;
}

wxString App::getDataDir()
{
  wxStandardPaths paths = wxStandardPaths::Get();
  return paths.GetDataDir();
}

wxIMPLEMENT_APP(App);
