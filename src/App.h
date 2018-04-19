/*
 * App.h
 *
 */

#ifndef APP_H_
#define APP_H_

#include <wx/app.h>

enum
{
    ID_MAINFRAME = wxID_HIGHEST + 1,
    ID_TOOLBAR,
    ID_CHESSBOARD,
    ID_GAMETEXT,
    ID_GAMESLIST,
    ID_CMD_FLIPBOARD,
};

class App: public wxApp
{
public:
    wxFrame *frame;

    virtual bool OnInit();
    static wxString getDataDir();
};

DECLARE_APP(App)

#endif /* APP_H_ */
