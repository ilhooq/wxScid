/*
 * GameTxtCtrl.h
 *
 */

#ifndef WIDGETS_GAMETXTCTRL_H_
#define WIDGETS_GAMETXTCTRL_H_

#include <wx/richtext/richtextctrl.h>
#include <wx/vector.h>
#include "database.h"

class GameTxtCtrl: public wxRichTextCtrl
{
public:
    enum STYLES {
        BASE,
        VARIANT,
        COMMENT,
        NAGS
    };

    GameTxtCtrl(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& point = wxDefaultPosition,
                const wxSize& size = wxDefaultSize);
    ~GameTxtCtrl()
    {
    }

    void Next();
    void Prev();

private:
    wxVector<GamePos> *game;
    wxVector<wxRichTextRange> *movesRange;
    wxRichTextAttr styles[4];
    int currentMove;
    void OnGameLoaded(wxCommandEvent& evt);
    void OnKeyDown(wxKeyEvent& evt);
    void OnURL(wxTextUrlEvent& evt);
    void WriteGame();
    void highLightMove(int move, bool activate);
    void PlayMove(int move);

wxDECLARE_EVENT_TABLE();
};

#endif /* WIDGETS_GAMETXTCTRL_H_ */
