/*
 * App.h
 *
 */

#ifndef APP_H_
#define APP_H_

#include <wx/app.h>
#include "scid/scid.h"
#include "database.h"

class App: public wxApp
{
public:
  wxFrame *frame;
  Scid* scid;
  int currentDbHandle;

  virtual bool OnInit();
  static wxString getDataDir();
  DbInfos OpenDatabase(wxString path);
};

DECLARE_APP(App)

#endif /* APP_H_ */
