/*
 * App.cpp
 *
 */

#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/stdpaths.h>

#include "App.h"
#include "database.h"
#include "MainFrame.h"
#include "Scid.h"

bool App::OnInit()
{
  // In order to fix APPNAME to wxcid in WX_${APPNAME}_DATA_DIR env var.
  SetAppName("wxscid");
  wxPrintf(wxT("Data dir: %s.\n"), getDataDir());

  // Make sure to call this first
  wxInitAllImageHandlers();
  wxXmlResource::Get()->InitAllHandlers();

  // Load all of the XRC files that will be used.
  if (!wxXmlResource::Get()->LoadAllFiles(getDataDir() + "/xrc")) {
    return false;
  }

  //currentDbHandle = 0;
  //scid = new Scid();

  frame = new MainFrame(NULL, MainFrame::ID, _T("WxScid"), wxDefaultPosition, wxSize(800, 600));
  frame->Show(true);

  frame->PushEventHandler(new Scid);

  SetTopWindow(frame);

  return true;
}

/*
DbInfos App::OpenDatabase(wxString path)
{
  try {
    currentDbHandle = scid->openDatabase(path.c_str());
  } catch(ScidError &error) {
    wxLogError(wxT("Error : %s - code : %d.\n"), error.what(), error.getCode());
    currentDbHandle = 0;
  }

  DbInfos infos;
  infos.handle = currentDbHandle;
  infos.gamesNumber = scid->numGames(currentDbHandle);
  infos.path = path;

  return infos;
}
*/

wxString App::getDataDir()
{
  wxStandardPaths paths = wxStandardPaths::Get();
  return paths.GetDataDir();
}

wxIMPLEMENT_APP(App);
