/*
 * MainFrame.h
 *
 */

#ifndef MAINFRAME_H_
#define MAINFRAME_H_

#include "wx/wx.h"
#include "wx/aui/aui.h"
#include "widgets/ChessBoard.h"
#include "scid/scid.h"

class MainFrame: public wxFrame
{
  enum
  {
    ID_Settings = wxID_HIGHEST + 1,
    ID_CustomizeToolbar,
    ID_Toolbar,
    ID_FLIPBOARD,
    ID_GAMES_LIST_VIEW,

    ID_CreatePerspective,
    ID_CopyPerspectiveCode,

    ID_FirstPerspective = ID_CreatePerspective + 1000
  };
public:
  MainFrame(wxWindow* parent,
    wxWindowID id,
    const wxString& title,
    const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize,
    long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);

  ~MainFrame();

  void flipBoard(wxCommandEvent & WXUNUSED(evt));

private:

  wxAuiManager auiManager;

  void OnExit(wxCommandEvent& evt);

  void OpenDatabase(wxCommandEvent& evt);

  wxDECLARE_EVENT_TABLE();

protected:
  ChessBoard* board;
  wxPanel* gamesList;
  wxPanel* moveTree;
  wxPanel* boardPanel;
  wxPanel* gameViewer;
};

#endif /* MAINFRAME_H_ */
