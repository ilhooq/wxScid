/*
 * GameTxtCtrl.cpp
 *
 */

#include "GameTxtCtrl.h"

wxDEFINE_EVENT(EVT_MOVE_TO_POSITION, wxCommandEvent);

BEGIN_EVENT_TABLE(GameTxtCtrl, wxRichTextCtrl)
    EVT_KEY_DOWN(GameTxtCtrl::OnKeyDown)
    EVT_TEXT_URL(wxID_ANY, GameTxtCtrl::OnURL)
END_EVENT_TABLE()

GameTxtCtrl::GameTxtCtrl(wxWindow* parent, wxWindowID id, const wxPoint &pos, const wxSize &size) :
wxRichTextCtrl(parent, id, "", pos, size, wxBORDER_NONE | wxWANTS_CHARS | wxRE_MULTILINE | wxRE_READONLY),
currentMove(0)
{
    movesRange = new wxVector<wxRichTextRange>;

    styles[BASE].SetFontUnderlined(false);
    styles[BASE].SetFontWeight(wxFONTWEIGHT_BOLD);
    styles[BASE].SetFontSize(11);
    styles[BASE].SetTextColour(wxColour(0, 0, 0, wxALPHA_OPAQUE));

    styles[VARIANT].SetFontUnderlined(false);
    styles[VARIANT].SetFontWeight(wxFONTWEIGHT_NORMAL);
    styles[VARIANT].SetTextColour(wxColour(39, 108, 151, wxALPHA_OPAQUE));

    styles[COMMENT].SetFontUnderlined(false);
    styles[COMMENT].SetFontWeight(wxFONTWEIGHT_NORMAL);
    styles[COMMENT].SetTextColour(wxColour(39, 165, 60, wxALPHA_OPAQUE));

    styles[NAGS].SetFontUnderlined(false);
    styles[NAGS].SetFontWeight(wxFONTWEIGHT_NORMAL);
    styles[NAGS].SetTextColour(wxColour(228, 0, 0, wxALPHA_OPAQUE));
}

void GameTxtCtrl::Next()
{
    PlayMove(currentMove+1);
}

void GameTxtCtrl::Prev()
{
    PlayMove(currentMove-1);
}

void GameTxtCtrl::OnKeyDown(wxKeyEvent& evt)
{
    long code = evt.GetKeyCode();

    switch (code) {
    case WXK_LEFT:
        PlayMove(currentMove-1);
        break;
    case WXK_RIGHT:
        PlayMove(currentMove+1);
        break;
    }
}

void GameTxtCtrl::OnURL(wxTextUrlEvent& evt)
{
    int moveIdx = wxAtoi(evt.GetString());
    PlayMove(moveIdx);
}

void GameTxtCtrl::WriteGame(wxVector<GamePos> &game)
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

    BeginStyle(styles[BASE]);

    int depth = 0;

    for (it = game.begin(); it != game.end(); it++, i++) {

        GamePos pos = *it;

        if (it != game.begin()) {
            WriteText(" ");
        }

        if (pos.RAVdepth > depth) {
            depth++;
            // Begin variant
            Newline();
            BeginLeftIndent(30 * pos.RAVdepth);
            BeginStyle(styles[VARIANT]);
            WriteText("(");
        } else if (pos.RAVdepth < depth) {
            // End variant
            WriteText(")");
            Newline();
            EndStyle();
            EndLeftIndent();
            depth--;
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
            BeginStyle(styles[NAGS]);
            WriteText(pos.NAGs);
            EndStyle();
        }

        if (pos.comment.Len()) {
            BeginStyle(styles[COMMENT]);
            WriteText(" ");
            WriteText(pos.comment);
            EndStyle();
        }
    }

    EndStyle();
    EndSuppressUndo();
    win->Thaw();
}

void GameTxtCtrl::highLightMove(int move, bool activate)
{
    if (movesRange->size() >= move) {
        wxRichTextAttr style;
        wxRichTextRange range = movesRange->at(move);
        GetStyleForRange(range, style);
        wxColour colour = (activate)? wxColor(220, 230, 230, wxALPHA_OPAQUE) : wxColor(255, 255, 255, wxALPHA_OPAQUE);
        style.SetBackgroundColour(colour);
        SetStyle(range, style);
    }
}

void GameTxtCtrl::ActivateMove(int move)
{
    highLightMove(currentMove, false);
    currentMove = move;
    highLightMove(currentMove, true);
}

void GameTxtCtrl::PlayMove(int move)
{
    ActivateMove(move);
    wxWindow *win = (wxWindow*) this;
    wxCommandEvent event(EVT_MOVE_TO_POSITION);
    event.SetEventObject(this);
    event.SetInt(move);
    win->ProcessWindowEvent(event);
}
