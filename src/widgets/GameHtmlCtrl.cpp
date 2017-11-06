/*
 * GameHtmlCtrl.cpp
 *
 */
#include <wx/html/htmlcell.h>
#include <wx/menu.h>

#include "GameHtmlCtrl.h"
#include "events.h"

BEGIN_EVENT_TABLE(GameHtmlCtrl, wxHtmlWindow)
    EVT_COMMAND (wxID_ANY, EVT_GAME_LOADED, GameHtmlCtrl::OnGameLoaded)
END_EVENT_TABLE()

GameHtmlCtrl::GameHtmlCtrl(wxWindow* parent,
                           wxWindowID id,
                           const wxPoint &pos,
                           const wxSize &size,
                           long style,
                           const wxString& name) :
wxHtmlWindow(parent, id, pos, size, style, name)
{
    game = NULL;
}

void GameHtmlCtrl::OnGameLoaded(wxCommandEvent& evt)
{
    game = (wxVector<GamePos> *) evt.GetClientData();
    LoadGame();
}

void GameHtmlCtrl::LoadGame()
{
    SetPage(wxT("<html><body></body></html>"));

    wxVector<GamePos>::iterator it;
    int i = 0;

    wxWindow *win = (wxWindow*) this;

    // Need to call win->Freeze / win->Thaw to avoid
    // performance issue when populate the control
    win->Freeze();

    for (it = game->begin(); it != game->end(); it++, i++) {

        GamePos pos = *it;

        if (it != game->begin()) {
            AppendToPage(wxT(" "));
        }

        wxString tag = wxString::Format(wxT("<a href=\"%d\">%s</a>"), i, pos.lastMoveSAN);

        AppendToPage(tag);
    }

    win->Thaw();
}

void GameHtmlCtrl::OnLinkClicked(const wxHtmlLinkInfo &link)
{
    const wxMouseEvent *mouseEvt = link.GetEvent();

    if (mouseEvt->RightUp()) {

        wxPrintf(wxT("Right clicked\n"));
        wxPoint p = mouseEvt->GetPosition();

        wxMenu menu;
        menu.Append(wxID_ABOUT, wxT("&About"));
        menu.AppendSeparator();
        menu.Append(wxID_EXIT, wxT("E&xit"));

        wxWindow *win = (wxWindow*) this;
        win->PopupMenu(&menu, p.x, p.y);

        return;
    }

    int moveIdx = wxAtoi(link.GetHref());
    GamePos pos = game->at(moveIdx);
    wxPrintf(wxT("Move index is %d - Move clicked is %s\n"), moveIdx, pos.lastMoveSAN);
}

