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
    GameTxtCtrl(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& point = wxDefaultPosition,
                const wxSize& size = wxDefaultSize);
    ~GameTxtCtrl()
    {
    }

private:
    wxVector<GamePos> *game;
    wxVector<wxRichTextRange> *movesRange;
    void OnGameLoaded(wxCommandEvent& evt);
    void OnKeyDown(wxKeyEvent& evt);
    void OnURL(wxTextUrlEvent& evt);
    void WriteGame();wxDECLARE_EVENT_TABLE();
};

#endif /* WIDGETS_GAMETXTCTRL_H_ */
