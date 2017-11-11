/*
 * GameTxtCtrl.cpp
 *
 */
#include <wx/dcbuffer.h>

#include "GameTxtCtrl.h"
#include "events.h"

BEGIN_EVENT_TABLE(GameTxtCtrl, wxRichTextCtrl) EVT_COMMAND (wxID_ANY, EVT_GAME_LOADED, GameTxtCtrl::OnGameLoaded)
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
}

void GameTxtCtrl::Next()
{
    //currentMove++;
    //highLightCurrentMove();
    PlayMove(currentMove+1);
}

void GameTxtCtrl::Prev()
{
    //currentMove--;
    //highLightCurrentMove();
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
        wxPrintf(wxT("Key left pressed\n"));
        break;
    case WXK_RIGHT:
        wxPrintf(wxT("Key right pressed\n"));
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
    /*
    GamePos pos = game->at(moveIdx);

    wxWindow *win = (wxWindow*) this;
    wxCommandEvent event(EVT_MAKE_MOVE);
    event.SetEventObject(this);
    event.SetClientData(&pos);
    win->ProcessWindowEvent(event);
    */
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

    wxRichTextAttr baseStyle;
    baseStyle.SetFontUnderlined(false);
    baseStyle.SetFontWeight(wxFONTWEIGHT_BOLD);
    baseStyle.SetFontSize(11);
    baseStyle.SetTextColour(wxColour(0, 0, 0, wxALPHA_OPAQUE));

    wxRichTextAttr variantStyle;
    variantStyle.SetFontUnderlined(false);
    variantStyle.SetFontWeight(wxFONTWEIGHT_NORMAL);
    variantStyle.SetTextColour(wxColour(39, 108, 151, wxALPHA_OPAQUE));

    wxRichTextAttr commentStyle;
    commentStyle.SetFontUnderlined(false);
    commentStyle.SetFontWeight(wxFONTWEIGHT_NORMAL);
    commentStyle.SetTextColour(wxColour(39, 165, 60, wxALPHA_OPAQUE));

    wxRichTextAttr NAGsStyle;
    NAGsStyle.SetFontUnderlined(false);
    NAGsStyle.SetFontWeight(wxFONTWEIGHT_NORMAL);
    NAGsStyle.SetTextColour(wxColour(228, 0, 0, wxALPHA_OPAQUE));

    BeginStyle(baseStyle);

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
            BeginStyle(variantStyle);
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
            BeginStyle(NAGsStyle);
            WriteText(pos.NAGs);
            EndStyle();
        }

        if (pos.comment.Len()) {
            BeginStyle(commentStyle);
            WriteText(" ");
            WriteText(pos.comment);
            EndStyle();
        }
    }

    EndStyle();
    EndSuppressUndo();
    win->Thaw();

    highLightCurrentMove();
}

void GameTxtCtrl::highLightCurrentMove()
{
    wxRichTextAttr style;
    wxRichTextRange range = movesRange->at(currentMove);

    if (GetStyleForRange(range, style)) {
        style.SetBackgroundColour(wxColor(220, 230, 230, wxALPHA_OPAQUE));
        SetStyle(range, style);
    }

    if (currentMove > 1) {
        range = movesRange->at(currentMove-1);
        if (GetStyleForRange(range, style)) {
            style.SetBackgroundColour(wxColor(255, 255, 255, wxALPHA_OPAQUE));
            SetStyle(range, style);
        }
    }
}

void GameTxtCtrl::PlayMove(int move)
{
    currentMove = move;
    highLightCurrentMove();

    GamePos pos = game->at(currentMove);

    wxWindow *win = (wxWindow*) this;
    wxCommandEvent event(EVT_MAKE_MOVE);
    event.SetEventObject(this);
    event.SetClientData(&pos);
    win->ProcessWindowEvent(event);
}



