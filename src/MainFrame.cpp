/*
 * MainFrame.cpp
 *
 */

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"
#include "wx/artprov.h"
#include "wx/aui/aui.h"
#include "wx/filedlg.h"

#include "widgets/ChessBoard.h"
#include "widgets/GamesListCtrl.h"
#include "MainFrame.h"
#include "App.h"
#include "Squares.h"
#include "scid/scid.h"

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_MENU(wxID_EXIT,  MainFrame::OnExit)
  EVT_MENU(XRCID("open_database"),  MainFrame::OpenDatabase)
  // EVT_UPDATE_UI(MainFrame::ID_FLIPBOARD, MainFrame::updateUI)
  EVT_MENU(MainFrame::ID_FLIPBOARD, MainFrame::flipBoard)
wxEND_EVENT_TABLE()

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

  // Create game viewer panel
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

  GamesListCtrl* listView = new GamesListCtrl(gamesList, ID_GAMES_LIST_VIEW, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_VIRTUAL);

  wxSizer *listSizer = new wxBoxSizer(wxHORIZONTAL);
  listSizer->Add(listView, 1, wxEXPAND);
  gamesList->SetSizerAndFit(listSizer);

  // "Commit" all changes made to wxAuiManager
  auiManager.Update();
}

void MainFrame::OpenDatabase(wxCommandEvent& WXUNUSED(evt))
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

    try {
      int dbHandle = wxGetApp().scid->openDatabase(path.c_str());
      GamesListCtrl * listCtrl = (GamesListCtrl *) wxWindow::FindWindowById(ID_GAMES_LIST_VIEW);
      listCtrl->SetItemCount((long) wxGetApp().scid->numGames(dbHandle));
      listCtrl->dbHandle = dbHandle;
      listCtrl->scid = wxGetApp().scid;
    } catch(ScidError &error) {
      wxMessageOutputStderr err;
      err.Printf(wxT("Error : %s - code : %d.\n"), error.what(), error.getCode());
    }

    SetTitle(wxString("Database - ") << OpenDialog->GetFilename()); // Set the Title to reflect the file open
  }

  OpenDialog->Destroy();
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
  auiManager.UnInit();
}

