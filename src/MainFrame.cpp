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
#include "App.h"
#include "MainFrame.h"

MainFrame::MainFrame(wxWindow* parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style) :
wxFrame(parent, id, title, pos, size, style)
{
    wxString dataDir = App::getDataDir();

    // Set frame icon
    SetIcon(wxIcon(dataDir + "/icons/scid.ico"));

    // Load the menubar from XRC
    SetMenuBar(wxXmlResource::Get()->LoadMenuBar(wxT("mainmenu")));

    // Binds events dynamically
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::flipBoard, this, ID_CMD_FLIPBOARD);

    // Tell wxAuiManager to manage this frame
    auiManager.SetManagedWindow(this);

    // Customize AUI look
    wxAuiDockArt* artProvider = auiManager.GetArtProvider();
    artProvider->SetMetric(wxAUI_DOCKART_SASH_SIZE, 1);
    artProvider->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 22);
    artProvider->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
    artProvider->SetMetric(wxAUI_DOCKART_PANE_BUTTON_SIZE, 16);
    artProvider->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE);

    // Toolbar
    wxAuiToolBar* toolbar = new wxAuiToolBar(this, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize,
            wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_HORIZONTAL);
    toolbar->SetToolBitmapSize(wxSize(48, 48));

    wxBitmap toolbar_bmp1 = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, wxSize(16, 16));
    wxBitmap flip_bmp = wxImage(dataDir + "/icons/flip.png");

    toolbar->AddTool(ID_TOOLBAR + 1, wxT("Disabled"), toolbar_bmp1);
    toolbar->AddTool(ID_CMD_FLIPBOARD, wxT("Flip board"), flip_bmp);
    toolbar->AddTool(ID_TOOLBAR + 8, wxT("Test"), toolbar_bmp1);
    toolbar->AddTool(ID_TOOLBAR + 9, wxT("Test"), toolbar_bmp1);
    toolbar->AddSeparator();
    toolbar->AddTool(ID_TOOLBAR + 10, wxT("Test"), toolbar_bmp1);
    toolbar->AddTool(ID_TOOLBAR + 11, wxT("Test"), toolbar_bmp1);
    toolbar->AddSeparator();
    toolbar->AddTool(ID_TOOLBAR + 12, wxT("Test"), toolbar_bmp1);
    toolbar->AddTool(ID_TOOLBAR + 13, wxT("Test"), toolbar_bmp1);
    toolbar->AddTool(ID_TOOLBAR + 14, wxT("Test"), toolbar_bmp1);
    toolbar->AddTool(ID_TOOLBAR + 15, wxT("Test"), toolbar_bmp1);
    toolbar->Realize();

    auiManager.AddPane(toolbar, wxAuiPaneInfo()
        .Name(wxT("main_toolbar"))
        .Caption(wxT("Main Toolbar"))
        .ToolbarPane()
        .Top()
    );

    // Create the board panel
    boardPanel = new wxPanel(this);

    // Init board
    board = new ChessBoard(boardPanel, ID_CHESSBOARD, dataDir + "/themes");
    // Setup initial position
    board->LoadPositionFromFen(wxT("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    board->Bind(wxEVT_MOUSEWHEEL, &MainFrame::OnMouseWheelOnBoard, this);

    // Expand panel contents
    wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(board, 1, wxEXPAND);
    boardPanel->SetSizerAndFit(sizer);

    auiManager.AddPane(boardPanel, wxAuiPaneInfo()
            .Name(wxT("board_panel"))
            .CenterPane()
            .BestSize(wxSize(800,800))
            .MinSize(wxSize(200,200))
    );

    // Create move Tree Panel
    moveTree = new wxPanel(this);
    moveTree->SetBackgroundColour(wxColour("#e9ddaf"));
    auiManager.AddPane(moveTree, wxAuiPaneInfo()
        .Right()
        .Name(wxT("move_tree"))
        .Caption(wxT("Move tree"))
        .DefaultPane()
        .BestSize(wxSize(600,400))
        .MinSize(wxSize(300,200))
    );

    // Create game text panel
    gameViewer = new wxPanel(this);
    auiManager.AddPane(gameViewer, wxAuiPaneInfo()
        .Right()
        .Name(wxT("game_viewer"))
        .Caption(wxT("Notation"))
        .DefaultPane()
        .BestSize(wxSize(600,400))
        .MinSize(wxSize(300,200))
    );

    GameTxtCtrl *txtCtrl = new GameTxtCtrl(gameViewer, ID_GAMETEXT);

    wxSizer *txtSizer = new wxBoxSizer(wxHORIZONTAL);
    txtSizer->Add(txtCtrl, 1, wxEXPAND);
    gameViewer->SetSizerAndFit(txtSizer);

    // Create game list panel
    gamesList = new wxPanel(this);
    auiManager.AddPane(gamesList, wxAuiPaneInfo()
        .Bottom()
        .DefaultPane()
        .Name(wxT("games_list"))
        .Caption(wxT("Games list"))
        .BestSize(wxSize(1900,600))
        .MinSize(wxSize(400,200))
        .MaximizeButton()
        .MinimizeButton(true)
    );

    GamesListCtrl* listCtrl = new GamesListCtrl(gamesList, ID_GAMESLIST);

    wxSizer *listSizer = new wxBoxSizer(wxHORIZONTAL);
    listSizer->Add(listCtrl, 1, wxEXPAND);
    gamesList->SetSizerAndFit(listSizer);

    // "Commit" all changes made to wxAuiManager
    auiManager.Update();
}

void MainFrame::OnMouseWheelOnBoard(wxMouseEvent& evt)
{
    GameTxtCtrl * textCtrl = (GameTxtCtrl *) wxWindow::FindWindowById(ID_GAMETEXT);

    int rot = evt.GetWheelRotation();
    if (rot > 0) {
        // Up
        textCtrl->Prev();
    } else {
        // Down
        textCtrl->Next();
    }
}

void MainFrame::OnExit(wxCommandEvent &WXUNUSED(evt))
{
    /*
     * The application normally shuts down when
     * the last of its top level windows is closed.
     */
    Close(true); // true is to force the frame to close.
}

void MainFrame::flipBoard(wxCommandEvent &WXUNUSED(evt))
{
    board->Flip();
}

MainFrame::~MainFrame()
{
    // Remove Scid EvntHandler
    PopEventHandler(true);
    auiManager.UnInit();
}

