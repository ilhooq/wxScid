/*
 * GameHtmlCtrl.h
 *
 */

#ifndef WIDGETS_GAMEHTMLCTRL_H_
#define WIDGETS_GAMEHTMLCTRL_H_


#include <wx/html/htmlwin.h>
#include <wx/vector.h>

#include "database.h"

class GameHtmlCtrl: public wxHtmlWindow
{
public:
    GameHtmlCtrl(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& point = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHW_SCROLLBAR_AUTO,
                const wxString& name = wxT("htmlGameWindow"));
    ~GameHtmlCtrl()
    {
    }

    virtual void OnLinkClicked (const wxHtmlLinkInfo &link);

private:
    wxVector<GamePos> *game;
    void OnGameLoaded(wxCommandEvent& evt);
    void OnKeyDown(wxKeyEvent& evt);

    void LoadGame();
wxDECLARE_EVENT_TABLE();
};



#endif /* WIDGETS_GAMEHTMLCTRL_H_ */
