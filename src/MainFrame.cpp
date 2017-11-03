/*
 * MainFrame.cpp
 *
 */
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/artprov.h>
#include <wx/aui/aui.h>
#include <wx/filedlg.h>

#include "widgets/ChessBoard.h"
#include "widgets/GamesListCtrl.h"
#include "widgets/GameTxtCtrl.h"
#include "database.h"
#include "events.h"
#include "App.h"
#include "MainFrame.h"
#include "Squares.h"

MainFrame::MainFrame(
  wxWindow* parent,
  wxWindowID id,
  const wxString& title,
  const wxPoint& pos,
  const wxSize& size,
  long style) : wxFrame(parent, id, title, pos, size, style)
{
  wxString dataDir = App::getDataDir();

  // Set frame icon
  SetIcon(wxIcon(dataDir + "/icons/scid.ico"));

  // Load the menubar from XRC
  SetMenuBar(wxXmlResource::Get()->LoadMenuBar(wxT("mainmenu")));

  // Binds events dynamically
  Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnExit, this, wxID_EXIT);
  Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnOpenDatabaseDialog, this, XRCID("open_database"));
  Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::flipBoard, this, MainFrame::ID_FLIPBOARD);

  Bind(EVT_OPEN_DATABASE, &MainFrame::OnOpenDatabase, this);

  // Tell wxAuiManager to manage this frame
  auiManager.SetManagedWindow(this);

  // Customize AUI look
  wxAuiDockArt* artProvider = auiManager.GetArtProvider();
  artProvider->SetMetric(wxAUI_DOCKART_SASH_SIZE, 1);
  artProvider->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 24);
  artProvider->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
  artProvider->SetMetric(wxAUI_DOCKART_PANE_BUTTON_SIZE, 16);
  artProvider->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE);

  // Toolbar
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

  wxAuiToolBar* toolbar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
      wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_HORIZONTAL);
  toolbar->SetToolBitmapSize(wxSize(48, 48));

  wxBitmap toolbar_bmp1 = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, wxSize(16, 16));
  wxBitmap flip_bmp = wxImage(dataDir + "/icons/flip.png");

  toolbar->AddTool(ID_Toolbar + 1, wxT("Disabled"), toolbar_bmp1);
  toolbar->AddTool(ID_FLIPBOARD, wxT("Flip board"), flip_bmp);
  toolbar->AddTool(ID_Toolbar + 8, wxT("Test"), toolbar_bmp1);
  toolbar->AddTool(ID_Toolbar + 9, wxT("Test"), toolbar_bmp1);
  toolbar->AddSeparator();
  toolbar->AddTool(ID_Toolbar + 10, wxT("Test"), toolbar_bmp1);
  toolbar->AddTool(ID_Toolbar + 11, wxT("Test"), toolbar_bmp1);
  toolbar->AddSeparator();
  toolbar->AddTool(ID_Toolbar + 12, wxT("Test"), toolbar_bmp1);
  toolbar->AddTool(ID_Toolbar + 13, wxT("Test"), toolbar_bmp1);
  toolbar->AddTool(ID_Toolbar + 14, wxT("Test"), toolbar_bmp1);
  toolbar->AddTool(ID_Toolbar + 15, wxT("Test"), toolbar_bmp1);
  toolbar->SetCustomOverflowItems(prepend_items, append_items);
  toolbar->Realize();

  auiManager.AddPane(toolbar, wxAuiPaneInfo()
      .Name(wxT("main_toolbar"))
      .Caption(wxT("Main Toolbar"))
      .ToolbarPane()
      .Top()
  );

  // Create the board panel
  boardPanel = new wxPanel(this);
  boardPanel->SetBackgroundColour(wxColour("#ffeeaa"));

  // Init board
  board = new ChessBoard(boardPanel, dataDir + "/themes");
  board->addPiece(ChessBoard::wRook, a1);
  board->addPiece(ChessBoard::wKing, e1);

  // Expand panel contents
  wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(board, 1, wxEXPAND);
  boardPanel->SetSizerAndFit(sizer);

  auiManager.AddPane(boardPanel, wxAuiPaneInfo().Name(wxT("board_panel")).CenterPane());

  // Create move Tree Panel
  moveTree = new wxPanel(this);
  moveTree->SetBackgroundColour(wxColour("#e9ddaf"));
  auiManager.AddPane(moveTree, wxAuiPaneInfo()
      .Left()
      .Name(wxT("move_tree"))
      .Caption(wxT("Move tree"))
      .DefaultPane()
      .BestSize(wxSize(200,100))
      .MinSize(wxSize(200,100))
  );

  // Create game text panel
  gameViewer = new wxPanel(this);
  gameViewer->SetBackgroundColour(wxColour("#f6ffd5"));
  auiManager.AddPane(gameViewer, wxAuiPaneInfo()
      .Right()
      .Name(wxT("game_viewer"))
      .Caption(wxT("Notation"))
      .DefaultPane()
      .BestSize(wxSize(600,200))
      .MinSize(wxSize(200,100))
  );

  GameTxtCtrl *txtCtrl = new GameTxtCtrl(gameViewer, ID_CTRL_GAME_TXT);

  wxSizer *txtSizer = new wxBoxSizer(wxHORIZONTAL);
  txtSizer->Add(txtCtrl, 1, wxEXPAND);
  gameViewer->SetSizerAndFit(txtSizer);

  // Create game list panel
  gamesList = new wxPanel(this);
  gamesList->SetBackgroundColour(wxColour("#f6ffd5"));
  auiManager.AddPane(gamesList, wxAuiPaneInfo()
      .Bottom()
      .DefaultPane()
      .Name(wxT("games_list"))
      .Caption(wxT("Games list"))
      .BestSize(wxSize(800,200))
      .MinSize(wxSize(400,100))
      .MaximizeButton()
      .MinimizeButton(true)
  );

  GamesListCtrl* listCtrl = new GamesListCtrl(gamesList, ID_GAMES_LIST_VIEW, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_VIRTUAL);

  wxSizer *listSizer = new wxBoxSizer(wxHORIZONTAL);
  listSizer->Add(listCtrl, 1, wxEXPAND);
  gamesList->SetSizerAndFit(listSizer);

  // "Commit" all changes made to wxAuiManager
  auiManager.Update();
}

void MainFrame::LoadGame(wxCommandEvent& evt)
{
  GameEntry *entry = (GameEntry*) evt.GetClientData();
  wxPrintf(wxT("Entry opened: %d\n"), entry->index);
}

void MainFrame::OnOpenDatabaseDialog(wxCommandEvent& WXUNUSED(evt))
{
  wxFileDialog* OpenDialog = new wxFileDialog (
   this,
   _("Choose a database to open"),
   wxEmptyString,
   wxEmptyString,
    _("Database files (*.si4, *.pgn)|*.si4;*.pgn"),
    wxFD_OPEN,
    wxDefaultPosition
  );

  if (OpenDialog->ShowModal() == wxID_OK)
  {
    wxString path = OpenDialog->GetPath();

    wxCommandEvent evt(EVT_OPEN_DATABASE_REQUEST, wxID_ANY);
    evt.SetEventObject(this);
    evt.SetString(path);
    ProcessWindowEvent(evt);
  }

  OpenDialog->Destroy();
}

void MainFrame::OnOpenDatabase(wxCommandEvent& evt)
{
  DbInfos *infos = (DbInfos*) evt.GetClientData();
  // Set the Title to reflect the file open
  SetTitle(wxString::Format(wxT("WxScid - %s (%d games)"), infos->name,  infos->numGames));
  wxWindow * listCtrl = (wxWindow *) wxWindow::FindWindowById(ID_GAMES_LIST_VIEW);
  listCtrl->ProcessWindowEvent(evt);
}

void MainFrame::OnExit(wxCommandEvent & WXUNUSED(evt))
{
  // true is to force the frame to close.
  Close(true);
}

void MainFrame::flipBoard(wxCommandEvent & WXUNUSED(evt))
{
  board->flip();
}

MainFrame::~MainFrame()
{
  // Remove Scid EvntHandler
  PopEventHandler(true);
  auiManager.UnInit();
}

