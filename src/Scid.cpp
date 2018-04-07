/*
 * Scid.cpp
 *
 */
#include <vector>
#include <map>
#include <wx/filename.h>

#include "scid/scid.h"

#include "database.h"
#include "Scid.h"

#include "events.h"
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
    EVT_COMMAND (wxID_ANY, EVT_OPEN_DATABASE_REQUEST, Scid::openDatabase)
    EVT_LIST_CACHE_HINT(wxID_ANY, Scid::OnGamesListCacheHint)
    EVT_LIST_ITEM_ACTIVATED (wxID_ANY, Scid::OnListItemActivated)
    EVT_COMMAND (wxID_ANY, EVT_DISPLAY_LIST_CELL, Scid::OnListDisplayCell)
    EVT_COMMAND (wxID_ANY, EVT_DROP_PIECE, Scid::OnDropPiece)
    EVT_COMMAND (wxID_ANY, EVT_LOAD_MOVE, Scid::OnLoadMove)
END_EVENT_TABLE()

Scid::Scid()
{
    scid::init();
    currentDbHandle = scid::base_getClipBaseHandle();
    // gameLoaded = new wxVector<GamePos>;
}

Scid::~Scid()
{
    scid::close();
}

void Scid::openDatabase(wxCommandEvent& evt)
{
    wxString path = evt.GetString();
    currentDbHandle = scid::base_open((std::string) path.c_str());

    DbInfos infos;
    infos.handle = currentDbHandle;
    infos.numGames = scid::base_numgames(currentDbHandle);
    infos.path = path;

    wxFileName fname((wxString) scid::base_filename(currentDbHandle));
    infos.name = fname.GetName();

    wxCommandEvent event(EVT_OPEN_DATABASE, wxID_ANY);
    event.SetEventObject(this);
    event.SetClientData(&infos);
    ProcessEvent(event);
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

    std::vector<scid::game_posInfos> dest;
    scid::base_getGame(currentDbHandle, entryIndex, dest);

    /*
    if (!gameLoaded->empty())
    {
        gameLoaded->clear();
    }
    */

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

    GameTxtCtrl * textCtrl = (GameTxtCtrl *) wxWindow::FindWindowById(MainFrame::ID_CTRL_GAME_TXT);
    ChessBoard * chessboard = (ChessBoard *) wxWindow::FindWindowById(MainFrame::ID_CHESSBOARD);

    if (game.size() > 0) {
        // Load First postion of the game
        GameTxtCtrl::GamePos pos = game.at(0);
        chessboard->LoadPositionFromFen(pos.FEN);
    }

    textCtrl->WriteGame(game);

    // wxCommandEvent event(EVT_GAME_LOADED, wxID_ANY);
    // event.SetEventObject(this);
    // event.SetClientData(gameLoaded);
    // ProcessEvent(event);
}

void Scid::OnDropPiece(wxCommandEvent& evt)
{
    PieceMove *move = (PieceMove *) evt.GetClientData();
    // Todo: Manage promotion
    move->canDrop = scid::move_add(currentDbHandle, move->from, move->to, 0);
}

void Scid::OnLoadMove(wxCommandEvent& evt)
{
    ChessBoard * chessboard = (ChessBoard *) wxWindow::FindWindowById(MainFrame::ID_CHESSBOARD);
    int move = evt.GetInt();
    scid::move_to(currentDbHandle, move);
    chessboard->LoadPositionFromFen(scid::pos_fen(currentDbHandle));
}
