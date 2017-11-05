/*
 * App.h
 *
 */

#ifndef APP_H_
#define APP_H_

#include <wx/app.h>

class App: public wxApp
{
public:
    wxFrame *frame;

    virtual bool OnInit();
    static wxString getDataDir();
};

DECLARE_APP(App)

#endif /* APP_H_ */
