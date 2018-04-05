/*
 * GameTxtCtrl.cpp
 *
 */
#include <wx/dcbuffer.h>

#include "GameTxtCtrl.h"
#include "events.h"

BEGIN_EVENT_TABLE(GameTxtCtrl, wxRichTextCtrl)
EVT_COMMAND (wxID_ANY, EVT_GAME_LOADED, GameTxtCtrl::OnGameLoaded)
EVT_KEY_DOWN(GameTxtCtrl::OnKeyDown)
EVT_TEXT_URL(wxID_ANY, GameTxtCtrl::OnURL)
// EVT_MOTION(GameTxtCtrl::OnMouseMove)
END_EVENT_TABLE()

GameTxtCtrl::GameTxtCtrl(wxWindow* parent, wxWindowID id, const wxPoint &pos, const wxSize &size) :
wxRichTextCtrl(parent, id, "", pos, size, wxBORDER_NONE | wxWANTS_CHARS | wxRE_MULTILINE | wxRE_READONLY),
currentMove(1)
{
    game = NULL;
    movesRange = new wxVector<wxRichTextRange>;
    // oldLinkHoverObject = NULL;

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

void GameTxtCtrl::OnGameLoaded(wxCommandEvent& evt)
{
    game = (wxVector<GamePos> *) evt.GetClientData();
    // oldLinkHoverObject = NULL;
    WriteGame();
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

/*** To Have onMouse over effect on links ***/

/*
void GameTxtCtrl::OnMouseMove(wxMouseEvent& event)
{
    wxWindow *win = (wxWindow*) this;

    wxClientDC dc(this);
    PrepareDC(dc);

    dc.SetFont(win->GetFont());

    long position = 0;
    wxPoint logicalPt = event.GetLogicalPosition(dc);
    wxRichTextObject* hitObj = NULL;
    wxRichTextObject* contextObj = NULL;

    int flags = 0;

    wxRichTextParagraphLayoutBox* container = & GetBuffer();
    wxRichTextDrawingContext context(& GetBuffer());

    int hit = container->HitTest(dc, context, GetUnscaledPoint(logicalPt), position, & hitObj, & contextObj, flags);

    // See if we need to change the cursor and the font underlined

    if (oldLinkHoverObject != NULL && hitObj != oldLinkHoverObject) {
        wxRichTextAttr oldAttr = oldLinkHoverObject->GetAttributes();

        if (oldAttr.GetFontUnderlined()) {
            oldAttr.SetFontUnderlined(false);
            SetStyle(oldLinkHoverObject, oldAttr);
        }
    }

    if (hit != wxRICHTEXT_HITTEST_NONE && !(hit & wxRICHTEXT_HITTEST_OUTSIDE) && hitObj)
    {
        wxRichTextParagraphLayoutBox* actualContainer = wxDynamicCast(contextObj, wxRichTextParagraphLayoutBox);
        if (actualContainer) {
            wxRichTextAttr attr = hitObj->GetAttributes();
            if (attr.HasFlag(wxTEXT_ATTR_URL) && !attr.GetFontUnderlined()) {
                attr.SetFontUnderlined(true);
                SetStyle(hitObj, attr);
                oldLinkHoverObject = hitObj;
            }

            ProcessMouseMovement(actualContainer, hitObj, position, logicalPt);
        }
    }
    else {
        win->SetCursor(m_textCursor);
    }

    event.Skip();
}
*/

void GameTxtCtrl::OnURL(wxTextUrlEvent& evt)
{
    int moveIdx = wxAtoi(evt.GetString());
    PlayMove(moveIdx);
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

    BeginStyle(styles[BASE]);

    int depth = 0;

    for (it = game->begin(); it != game->end(); it++, i++) {

        GamePos pos = *it;

        if (it != game->begin()) {
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

    highLightMove(currentMove, true);
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

void GameTxtCtrl::PlayMove(int move)
{
    if (move > 0 && move < game->size()) {

        highLightMove(currentMove, false);
        currentMove = move;
        highLightMove(currentMove, true);

        GamePos pos = game->at(currentMove);

        wxWindow *win = (wxWindow*) this;
        wxCommandEvent event(EVT_MAKE_MOVE);
        event.SetEventObject(this);
        event.SetClientData(&pos);
        win->ProcessWindowEvent(event);
    }
}



