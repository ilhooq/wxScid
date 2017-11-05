/*
 * GameTxtCtrl.cpp
 *
 */
#include "GameTxtCtrl.h"
#include "events.h"

BEGIN_EVENT_TABLE(GameTxtCtrl, wxRichTextCtrl) EVT_COMMAND (wxID_ANY, EVT_GAME_LOADED, GameTxtCtrl::OnGameLoaded)
EVT_KEY_DOWN(GameTxtCtrl::OnKeyDown)
EVT_TEXT_URL(wxID_ANY, GameTxtCtrl::OnURL)
END_EVENT_TABLE()

GameTxtCtrl::GameTxtCtrl(wxWindow* parent, wxWindowID id, const wxPoint &pos, const wxSize &size) :
wxRichTextCtrl(parent, id, "", pos, size, wxBORDER_NONE | wxWANTS_CHARS | wxRE_MULTILINE | wxRE_READONLY)
{
    game = NULL;
    movesRange = new wxVector<wxRichTextRange>;
}

void GameTxtCtrl::OnGameLoaded(wxCommandEvent& evt)
{
    game = (wxVector<GamePos> *) evt.GetClientData();
    WriteGame();
}

void GameTxtCtrl::OnKeyDown(wxKeyEvent& evt)
{
    long code = evt.GetKeyCode();

    switch (code) {
    case WXK_LEFT:
        wxPrintf(wxT("Key left pressed\n"));
        break;
    case WXK_RIGHT:
        wxPrintf(wxT("Key right pressed\n"));
        break;
    }
}

void GameTxtCtrl::OnURL(wxTextUrlEvent& evt)
{
    int moveIdx = wxAtoi(evt.GetString());
    GamePos pos = game->at(moveIdx);
    wxPrintf(wxT("Move index is %d - Move clicked is %s\n"), moveIdx, pos.lastMoveSAN);
}

void GameTxtCtrl::WriteGame()
{
    Clear();
    if (!movesRange->empty()) {
        movesRange->clear();
    }

    wxVector<GamePos>::iterator it;
    int i = 0;

    wxWindow *win = (wxWindow*) this;

    win->GetCaret()->Show(false);

    // Need to call win->Freeze / win->Thaw to avoid
    // performance issue when populate the text control
    win->Freeze();

    BeginSuppressUndo();
    BeginBold();

    for (it = game->begin(); it != game->end(); it++, i++) {

        GamePos pos = *it;

        if (it != game->begin()) {
            WriteText(" ");
        }

        wxString url;
        BeginURL(url << i);
        long begin = GetLastPosition();
        WriteText(pos.lastMoveSAN);

        long end = GetLastPosition();
        EndURL();

        wxRichTextRange range(begin, end);
        movesRange->push_back(range);

        if (pos.NAGs.Len()) {
            WriteText(pos.NAGs);
        }
    }

    EndSuppressUndo();
    win->Thaw();

    wxPrintf(wxT("\n"));
}

