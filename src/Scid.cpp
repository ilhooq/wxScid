/*
 * Scid.cpp
 *
 */
#include <vector>
#include <map>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/xrc/xmlres.h>

#include "scid/scid.h"
#include "App.h"
#include "Scid.h"
#include "MainFrame.h"
#include "widgets/ChessBoard.h"
#include "widgets/GamesListCtrl.h"
#include "widgets/GameTxtCtrl.h"

static std::map<unsigned int, scid::game_entry> entriesMap;

bool static mapEntryExists(long item)
{
    std::map<unsigned int, scid::game_entry>::iterator it = entriesMap.find(item);
    return (it != entriesMap.end());
}

BEGIN_EVENT_TABLE(Scid, wxEvtHandler)
    EVT_MENU(XRCID("ID_OPEN_DATABASE"), Scid::OnOpenDatabase)
    EVT_MENU(ID_CMD_FLIPBOARD, Scid::OnCmdFlipBoard)
    EVT_LIST_CACHE_HINT(wxID_ANY, Scid::OnGamesListCacheHint)
    EVT_LIST_ITEM_ACTIVATED (wxID_ANY, Scid::OnListItemActivated)
    EVT_COMMAND (wxID_ANY, EVT_DISPLAY_LIST_CELL, Scid::OnListDisplayCell)
    EVT_COMMAND (wxID_ANY, EVT_DROP_PIECE, Scid::OnDropPiece)
    EVT_COMMAND (wxID_ANY, EVT_MOVE_TO_POSITION, Scid::OnMoveToPosition)
END_EVENT_TABLE()

Scid::Scid()
{
    scid::init();
    currentDbHandle = scid::base_getClipBaseHandle();
}

Scid::~Scid()
{
    scid::close();
}

void Scid::OnOpenDatabase(wxCommandEvent& evt)
{
    MainFrame * mainFrame = (MainFrame *) wxWindow::FindWindowById(ID_MAINFRAME);

    wxFileDialog* OpenDialog = new wxFileDialog (
        mainFrame,
        _("Choose a database to open"),
        wxEmptyString,
        wxEmptyString,
        _("Database files (*.si4, *.pgn)|*.si4;*.pgn"),
        wxFD_OPEN,
        wxDefaultPosition
    );

    if (OpenDialog->ShowModal() == wxID_OK) {

        wxString path = OpenDialog->GetPath();
        currentDbHandle = scid::base_open((std::string) path.c_str());

        unsigned int numGames = scid::base_numgames(currentDbHandle);
        wxFileName fname((wxString) scid::base_filename(currentDbHandle));
        wxString name = fname.GetName();

        // Set the Title to reflect the file open
        mainFrame->SetTitle(wxString::Format(wxT("WxScid - %s (%d games)"), name, numGames));
        GamesListCtrl * listCtrl = (GamesListCtrl *) wxWindow::FindWindowById(ID_GAMESLIST);
        listCtrl->SetItemCount(numGames);
    }

    OpenDialog->Destroy();
}

// Called on double click or when pressed ENTER on a row
void Scid::OnListItemActivated(wxListEvent &event)
{
    wxASSERT(mapEntryExists(event.GetIndex()));
    scid::game_entry entry = entriesMap[event.GetIndex()];
    LoadGame(entry.index);
}

// This function is called during the window OnPaint event
void Scid::OnGamesListCacheHint(wxListEvent& event)
{
    if (entriesMap.size() > 100000) {
        entriesMap.clear();
    }

    int count = event.GetCacheTo() - event.GetCacheFrom();

    // Add padding to retrieve more items in the cache
    count += 100;

    if (mapEntryExists(event.GetCacheFrom()) && mapEntryExists(count)) {
        // Items already exists
        return;
    }

    scid::base_gameslist(currentDbHandle, "d+", "dbfilter", event.GetCacheFrom(), count, &entriesMap);
}

void Scid::OnListDisplayCell(wxCommandEvent& evt)
{
    GameListCellData *data = (GameListCellData*) evt.GetClientData();

    std::map<unsigned int, scid::game_entry>::iterator it = entriesMap.find(data->item);

    if (it == entriesMap.end()) {
        // Entry not found
        data->text = "Error";
    }

    scid::game_entry entry = it->second;

    switch (data->column) {
        case GamesListCtrl::COL_DATE:
            data->text = (wxString) entry.date;
            break;
        case GamesListCtrl::COL_RESULT:
            data->text = (wxString) entry.result;
            break;
        case GamesListCtrl::COL_WHITENAME:
            data->text = wxString::FromUTF8(entry.whiteName.c_str());
            break;
        case GamesListCtrl::COL_WHITEELO:
            data->text = (wxString) entry.whiteElo;
            break;
        case GamesListCtrl::COL_BLACKNAME:
            data->text = wxString::FromUTF8(entry.blackName.c_str());
            break;
        case GamesListCtrl::COL_BLACKELO:
            data->text = (wxString) entry.blackElo;
            break;
        case GamesListCtrl::COL_FIRSTMOVES:
            data->text = (wxString) entry.firstMoves;;
            break;
    }
}

void Scid::LoadGame(unsigned int entryIndex)
{
    scid::game_load(currentDbHandle, entryIndex);

    /*
    std::vector<std::string> dest1;

    scid::game_moves(currentDbHandle, entryIndex, dest1);
    std::vector<std::string>::iterator it1;

    for (it1 = dest1.begin(); it1 != dest1.end(); it1++) {
        // scid::game_posInfos ScidPos = *it;
        wxPrintf("%s ", *it1);
    }
    wxPrintf("\n ");
    */


    std::string pgn = scid::game_pgn(currentDbHandle, entryIndex);
    wxPrintf("%s\n ", (wxString) pgn);


    std::vector<scid::game_posInfos> dest;

    scid::base_getGame(currentDbHandle, entryIndex, dest);

    wxVector<GameTxtCtrl::GamePos> game;

    std::vector<scid::game_posInfos>::iterator it;

    for (it = dest.begin(); it != dest.end(); it++) {
        scid::game_posInfos ScidPos = *it;
        GameTxtCtrl::GamePos pos;
        pos.RAVdepth = ScidPos.RAVdepth;
        pos.RAVnum = ScidPos.RAVnum;
        pos.NAGs = ScidPos.NAGs;
        pos.FEN = ScidPos.FEN;
        pos.comment = wxString::FromUTF8(ScidPos.comment.c_str());
        pos.comment.Replace(wxT("\n"), wxT(" "));
        pos.comment.Replace(wxT("\r"), wxT(""));
        pos.lastMoveSAN = ScidPos.lastMoveSAN;

        game.push_back(pos);
    }

    GameTxtCtrl * textCtrl = (GameTxtCtrl *) wxWindow::FindWindowById(ID_GAMETEXT);
    ChessBoard * chessboard = (ChessBoard *) wxWindow::FindWindowById(ID_CHESSBOARD);

    if (game.size() > 0) {
        // Load First postion of the game
        GameTxtCtrl::GamePos pos = game.at(0);
        chessboard->LoadPositionFromFen(pos.FEN);
    }

    textCtrl->WriteGame(game);
}

static int movePosition = 0;

void Scid::OnDropPiece(wxCommandEvent& evt)
{
    PieceMove *move = (PieceMove *) evt.GetClientData();
    // Todo: Manage promotion
    int promo = 0;

    move->canDrop = scid::pos_canMove(currentDbHandle, move->from, move->to, promo);

    if (move->canDrop) {
        movePosition++;

        if (scid::move_edge(currentDbHandle, scid::AT_END) || scid::move_edge(currentDbHandle, scid::AT_VAR_END)) {
            scid::move_add(currentDbHandle, move->from, move->to, 0);
        } else {

            scid::move_forward(currentDbHandle);

            if (!scid::move_isEqual(currentDbHandle, move->from, move->to, promo)) {

                scid::move_addVariation(currentDbHandle);
                scid::move_add(currentDbHandle, move->from, move->to, 0);
                // scid::move_exitVariation(currentDbHandle);
                movePosition++;

            }
        }

        // int position = scid::move_getPosition(currentDbHandle);

        wxPrintf("movePosition after OnDropPiece: %d\n", movePosition);

        std::vector<scid::game_posInfos> dest;
        scid::base_getGame(currentDbHandle, dest);
        wxVector<GameTxtCtrl::GamePos> game;

        std::vector<scid::game_posInfos>::iterator it;

        for (it = dest.begin(); it != dest.end(); it++) {
            scid::game_posInfos ScidPos = *it;
            GameTxtCtrl::GamePos pos;
            pos.RAVdepth = ScidPos.RAVdepth;
            pos.RAVnum = ScidPos.RAVnum;
            pos.NAGs = ScidPos.NAGs;
            pos.FEN = ScidPos.FEN;
            pos.comment = wxString::FromUTF8(ScidPos.comment.c_str());
            pos.comment.Replace(wxT("\n"), wxT(" "));
            pos.comment.Replace(wxT("\r"), wxT(""));
            pos.lastMoveSAN = ScidPos.lastMoveSAN;

            game.push_back(pos);
        }

        GameTxtCtrl * textCtrl = (GameTxtCtrl *) wxWindow::FindWindowById(ID_GAMETEXT);
        textCtrl->WriteGame(game);
        textCtrl->ActivateMove(movePosition);
    }
}

void Scid::OnMoveToPosition(wxCommandEvent& evt)
{
    ChessBoard * chessboard = (ChessBoard *) wxWindow::FindWindowById(ID_CHESSBOARD);
    int move = evt.GetInt();
    movePosition = move;
    wxPrintf("movePosition in OnMoveToPosition: %d\n", move);
    scid::move_to(currentDbHandle, move);
    chessboard->LoadPositionFromFen(scid::pos_fen(currentDbHandle));
}

void Scid::OnCmdFlipBoard(wxCommandEvent &evt)
{
    ChessBoard * chessboard = (ChessBoard *) wxWindow::FindWindowById(ID_CHESSBOARD);
    chessboard->Flip();
}
