/*
 * App.h
 *
 */

#ifndef APP_H_
#define APP_H_

#include "wx/wx.h"
#include "wx/msgout.h"
#include "scid/scid.h"

class App: public wxApp
{
public:
  wxFrame *frame;
  Scid* scid;
  virtual bool OnInit();
  static wxString getDataDir();
};

DECLARE_APP(App)

#endif /* APP_H_ */
