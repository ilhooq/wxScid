/*
 * GameTxtCtrl.h
 *
 */

#ifndef WIDGETS_GAMETXTCTRL_H_
#define WIDGETS_GAMETXTCTRL_H_

#include <wx/richtext/richtextctrl.h>
#include <wx/vector.h>

wxDECLARE_EVENT(EVT_LOAD_MOVE, wxCommandEvent);

class GameTxtCtrl: public wxRichTextCtrl
{
public:
    enum STYLES {
        BASE,
        VARIANT,
        COMMENT,
        NAGS
    };

    struct GamePos
    {
        int RAVdepth;
        int RAVnum;
        wxString FEN;
        wxString NAGs;
        wxString comment;
        wxString lastMoveSAN;
    };

    GameTxtCtrl(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& point = wxDefaultPosition,
                const wxSize& size = wxDefaultSize);
    ~GameTxtCtrl()
    {
    }

    void WriteGame(wxVector<GamePos> &game);
    void ActivateMove(int move);
    void PlayMove(int move);
    void Next();
    void Prev();

private:
    wxVector<wxRichTextRange> *movesRange;
    wxRichTextAttr styles[4];
    int currentMove;
    void OnKeyDown(wxKeyEvent& evt);
    void OnURL(wxTextUrlEvent& evt);
    void highLightMove(int move, bool activate);

wxDECLARE_EVENT_TABLE();
};

#endif /* WIDGETS_GAMETXTCTRL_H_ */
