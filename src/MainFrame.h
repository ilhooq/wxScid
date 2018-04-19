/*
 * MainFrame.h
 *
 */

#ifndef MAINFRAME_H_
#define MAINFRAME_H_

#include <wx/aui/aui.h>
#include <wx/frame.h>
#include "widgets/ChessBoard.h"

class MainFrame: public wxFrame
{
public:
    MainFrame(wxWindow* parent,
              wxWindowID id,
              const wxString& title,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxSize(800, 600),
              long style = wxDEFAULT_FRAME_STYLE | wxMAXIMIZE| wxBORDER_SUNKEN);

    ~MainFrame();

    void flipBoard(wxCommandEvent & WXUNUSED(evt));

private:
    wxAuiManager auiManager;
    void OnExit(wxCommandEvent& evt);
    void OnListGames(wxCommandEvent& evt);
    void OnGameLoaded(wxCommandEvent& evt);
    void OnMakeMove(wxCommandEvent& evt);
    void OnMouseWheelOnBoard(wxMouseEvent& evt);

protected:
    ChessBoard* board;
    wxPanel* gamesList;
    wxPanel* moveTree;
    wxPanel* boardPanel;
    wxPanel* gameViewer;
};

#endif /* MAINFRAME_H_ */
