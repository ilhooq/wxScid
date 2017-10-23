/*
 * MainFrame.cpp
 *
 */

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"
#include "wx/artprov.h"
#include "wx/aui/aui.h"

#include "widgets/ChessBoard.h"
#include "MainFrame.h"
#include "App.h"
#include "Squares.h"

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_MENU(wxID_EXIT,  MainFrame::OnExit)
  // EVT_UPDATE_UI(MainFrame::ID_FLIPBOARD, MainFrame::updateUI)
  EVT_MENU(MainFrame::ID_FLIPBOARD, MainFrame::flipBoard)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(
  wxWindow* parent,
  wxWindowID id,
  const wxString& title,
  const wxPoint& pos,
  const wxSize& size,
  long style) : wxFrame(parent, id, title, pos, size, style),
board(this, App::getDataDir() + "/themes")
{
  wxString dataDir = App::getDataDir();

  // Set frame icon
  SetIcon(wxIcon(dataDir + "/icons/scid.ico"));


  // Load the menubar from XRC.
  SetMenuBar(wxXmlResource::Get()->LoadMenuBar(wxT("mainmenu")));

  // Tell wxAuiManager to manage this frame
  auiManager.SetManagedWindow(this);

  // wxBoxSizer* box = new wxBoxSizer(wxVERTICAL);


  board.addPiece(ChessBoard::wRook, a1);
  board.addPiece(ChessBoard::wKing, e1);

  // prepare a few custom overflow elements for the toolbars' overflow buttons

  wxAuiToolBarItemArray prepend_items;
  wxAuiToolBarItemArray append_items;
  wxAuiToolBarItem item;
  item.SetKind(wxITEM_SEPARATOR);
  append_items.Add(item);
  item.SetKind(wxITEM_NORMAL);
  item.SetId(ID_CustomizeToolbar);
  item.SetLabel(_("Customize..."));
  append_items.Add(item);

  wxAuiToolBar* tb2 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
      wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_HORIZONTAL);
  tb2->SetToolBitmapSize(wxSize(16, 16));

  wxBitmap tb2_bmp1 = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, wxSize(16, 16));
  wxBitmap flip_bmp = wxImage(dataDir + "/icons/flip.png");

  tb2->AddTool(ID_Toolbar + 1, wxT("Disabled"), tb2_bmp1);
  tb2->AddTool(ID_FLIPBOARD, wxT("Flip board"), flip_bmp);
  tb2->AddTool(ID_Toolbar + 8, wxT("Test"), tb2_bmp1);
  tb2->AddTool(ID_Toolbar + 9, wxT("Test"), tb2_bmp1);
  tb2->AddSeparator();
  tb2->AddTool(ID_Toolbar + 10, wxT("Test"), tb2_bmp1);
  tb2->AddTool(ID_Toolbar + 11, wxT("Test"), tb2_bmp1);
  tb2->AddSeparator();
  tb2->AddTool(ID_Toolbar + 12, wxT("Test"), tb2_bmp1);
  tb2->AddTool(ID_Toolbar + 13, wxT("Test"), tb2_bmp1);
  tb2->AddTool(ID_Toolbar + 14, wxT("Test"), tb2_bmp1);
  tb2->AddTool(ID_Toolbar + 15, wxT("Test"), tb2_bmp1);
  tb2->SetCustomOverflowItems(prepend_items, append_items);
  //tb2->EnableTool(ID_SampleItem+6, false);
  tb2->Realize();

  wxAuiToolBar* tb5 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
      wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_VERTICAL);
  tb5->SetToolBitmapSize(wxSize(48, 48));
  tb5->AddTool(ID_Toolbar + 30, wxT("Test"), wxArtProvider::GetBitmap(wxART_ERROR));
  tb5->AddSeparator();
  tb5->AddTool(ID_Toolbar + 31, wxT("Test"), wxArtProvider::GetBitmap(wxART_QUESTION));
  tb5->AddTool(ID_Toolbar + 32, wxT("Test"), wxArtProvider::GetBitmap(wxART_INFORMATION));
  tb5->AddTool(ID_Toolbar + 33, wxT("Test"), wxArtProvider::GetBitmap(wxART_WARNING));
  tb5->AddTool(ID_Toolbar + 34, wxT("Test"), wxArtProvider::GetBitmap(wxART_MISSING_IMAGE));
  tb5->SetCustomOverflowItems(prepend_items, append_items);
  tb5->Realize();

  auiManager.AddPane(&board,
      wxAuiPaneInfo().Name(wxT("chessboard")).Caption(wxT("Pane Caption")).CenterPane().CloseButton(
          true).MaximizeButton(true));

  // add the toolbars to the manager
  auiManager.AddPane(tb2,
      wxAuiPaneInfo().Name(wxT("tb2")).Caption(wxT("Big Toolbar")).ToolbarPane().Top());

  auiManager.AddPane(tb5,
      wxAuiPaneInfo().Name(wxT("tb5")).Caption(wxT("Sample Vertical Toolbar")).ToolbarPane().Left().GripperTop());

  // auiManager.GetPane(wxT("chessboard")).Show();

  //frame->CreateStatusBar();
  //frame->SetStatusText(_T("Hello World"));

  // box->Add(board, 1, wxEXPAND);

  // SetSizer(box);

  // "commit" all changes made to wxAuiManager
  auiManager.Update();
}

void MainFrame::OnExit(wxCommandEvent & WXUNUSED(evt))
{
  // true is to force the frame to close.
  Close(true);
}

void MainFrame::flipBoard(wxCommandEvent & WXUNUSED(evt))
{
  board.flip();
}

MainFrame::~MainFrame()
{
  auiManager.UnInit();
}

