/*
 * Scid.cpp
 *
 */
#include <vector>
#include <wx/filename.h>

#include "scid/scidbase.h"
#include "scid/dbasepool.h"

#include "database.h"
#include "Scid.h"
#include "events.h"

BEGIN_EVENT_TABLE(Scid, wxEvtHandler) EVT_COMMAND (wxID_ANY, EVT_OPEN_DATABASE_REQUEST, Scid::openDatabase)
EVT_COMMAND (wxID_ANY, EVT_LISTGAMES_REQUEST, Scid::OnListGames)
EVT_COMMAND (wxID_ANY, EVT_LOAD_GAME_REQUEST, Scid::LoadGame)
END_EVENT_TABLE()

Scid::Scid()
{
    DBasePool::init();
    currentDbHandle = 0;
    gameLoaded = new wxVector<GamePos>;
}

Scid::~Scid()
{
    DBasePool::closeAll();
}

void Scid::openDatabase(wxCommandEvent& evt)
{
    wxString path = evt.GetString();
    std::string filename = (std::string) path.c_str();

    ICodecDatabase::Codec codec = ICodecDatabase::Codec::MEMORY;
    fileModeT fMode = FMODE_Both;

    std::string extension = filename.substr(filename.find_last_of(".") + 1);

    if (extension == "si4") {
        codec = ICodecDatabase::Codec::SCID4;
        // Remove the extension because scid database contains 3 files (*.si4, *.sg4, *.sn4)
        filename = filename.substr(0, filename.find_last_of("."));
    } else if (extension == "pgn") {
        codec = ICodecDatabase::Codec::PGN;
    } else {
        throw ScidError("Unrecognized file format.", ERROR_CodecUnsupFeat);
    }

    if (DBasePool::find(filename.c_str()) != 0) {
        throw ScidError("File already in use.", ERROR_FileInUse);
    }

    scidBaseT* dbase = DBasePool::getFreeSlot();

    if (dbase == 0) {
        throw ScidError("The database opening limit is reached", ERROR_Full);
    }

    Progress progress;

    errorT err = dbase->Open(codec, fMode, filename.c_str(), progress);

    if (err != OK && err != ERROR_NameDataLoss && fMode == FMODE_Both) {
        err = dbase->Open(ICodecDatabase::Codec::SCID4, FMODE_ReadOnly, filename.c_str(), progress);
    }

    progress.report(1, 1);

    if (err != OK && err != ERROR_NameDataLoss) {
        throw ScidError("Unable to open database", err);
    }

    currentDbHandle = DBasePool::switchCurrent(dbase);

    DbInfos infos;
    infos.handle = currentDbHandle;
    infos.numGames = dbase->numGames();
    infos.path = path;

    wxFileName fname((wxString) dbase->getFileName());
    infos.name = fname.GetName();

    wxCommandEvent event(EVT_OPEN_DATABASE, wxID_ANY);
    event.SetEventObject(this);
    event.SetClientData(&infos);
    ProcessEvent(event);
}

void Scid::OnListGames(wxCommandEvent& evt)
{
    ListGamesRequest *data = (ListGamesRequest*) evt.GetClientData();
    listGames(currentDbHandle, "d+", "dbfilter", data->HashEntries, data->fromItem, data->count);
}

void Scid::listGames(int baseHandle, const char* ordering, const char* filterId, HashGameEntries* hashEntries, unsigned int start, unsigned int count)
{
    scidBaseT* dbase = DBasePool::getBase(baseHandle);

    if (count == 0) {
        // Get all games
        count = dbase->numGames();
    }

    const HFilter filter = dbase->getFilter(filterId);

    if (filter == NULL) {
        throw ScidError("Filter bad arguments", ERROR_BadArg);
    }

    gamenumT* idxList = new gamenumT[count];

    count = dbase->listGames(ordering, (size_t) start, (size_t) count, filter, idxList);

    // The name base file in memory.
    const NameBase* nb = dbase->getNameBase();

    for (uint i = 0, item = start; i < count; ++i, item++) {

        uint idx = idxList[i];
        wxASSERT(filter->get(idx) != 0);
        GameEntry entry;

        uint ply = filter->get(idx) - 1;

        const IndexEntry* ie = dbase->getIndexEntry(idx);

        entry.result = RESULT_STR[ie->GetResult()];
        entry.movesNumber = (ie->GetNumHalfMoves() + 1) / 2;
        entry.whiteName = wxString::FromUTF8(ie->GetWhiteName(nb));
        entry.blackName = wxString::FromUTF8(ie->GetBlackName(nb));

        std::string eloStr;
        eloT welo = ie->GetWhiteElo();

        if (welo != 0) {
            eloStr = to_string(welo);
        } else {
            welo = ie->GetWhiteElo(nb);
            eloStr = to_string(welo);
            if (welo != 0) {
                eloStr.insert(eloStr.begin(), '(');
                eloStr.insert(eloStr.end(), ')');
            }
        }

        entry.whiteElo = (wxString) eloStr;

        eloT belo = ie->GetBlackElo();

        if (belo != 0) {
            eloStr = to_string(belo);
        } else {
            belo = ie->GetBlackElo(nb);
            eloStr = to_string(belo);
            if (belo != 0) {
                eloStr.insert(eloStr.begin(), '(');
                eloStr.insert(eloStr.end(), ')');
            }
        }

        entry.blackElo = (wxString) eloStr;

        char date[16];
        date_DecodeToString(ie->GetDate(), date);
        entry.date = (wxString) date;

        entry.eventName = (wxString) ie->GetEventName(nb);
        entry.roundName = (wxString) ie->GetRoundName(nb);
        entry.siteName = (wxString) ie->GetSiteName(nb);
        entry.nagCount = ie->GetNagCount();
        entry.commentCount = ie->GetCommentCount();
        entry.variationCount = ie->GetVariationCount();
        entry.deletedFlag = ie->GetDeleteFlag();

        char flags[16];
        ie->GetFlagStr(flags, "WBMENPTKQ!?U123456");
        entry.flags = (wxString) flags;

        char eco[6];
        eco_ToExtendedString(ie->GetEcoCode(), eco);
        entry.eco = (wxString) eco;

        entry.endMaterial = (wxString) matsig_makeString(ie->GetFinalMatSig());
        entry.startFlag = ie->GetStartFlag();

        char event_date[16];
        date_DecodeToString(ie->GetEventDate(), event_date);
        entry.eventDate = (wxString) event_date;

        entry.year = ie->GetYear();
        entry.rating = ie->GetRating(nb);
        FastGame game = dbase->getGame(ie);
        entry.firstMoves = (wxString) game.getMoveSAN(ply, 10);
        entry.index = idx;
        entry.ply = ply;

        (*hashEntries)[item] = entry;
    }

    delete[] idxList;

}

unsigned int Scid::numGames(int baseHandle)
{
    scidBaseT* dbase = DBasePool::getBase(baseHandle);
    return dbase->numGames();
}

void Scid::LoadGame(wxCommandEvent& evt)
{
    scidBaseT* dbase = DBasePool::getBase(currentDbHandle);

    GameEntry *entry = (GameEntry*) evt.GetClientData();

    const IndexEntry* ie = dbase->getIndexEntry(entry->index);

    // The name base file in memory.
    const NameBase* nb = dbase->getNameBase();
    wxPrintf(wxT("Enrtry requested: %d - Entry opened: %d - White : %s \n"), entry->index, ie->GetOffset(), wxString::FromUTF8(ie->GetWhiteName(nb)));

    std::vector<scidBaseT::GamePos> dest;
    dbase->getGame(ie, dest);

    std::vector<scidBaseT::GamePos>::iterator it;

    //wxVector<GamePos> *result = new wxVector<GamePos>;
    if (!gameLoaded->empty())
        gameLoaded->clear();

    for (it = dest.begin(); it != dest.end(); it++) {
        scidBaseT::GamePos ScidPos = *it;
        GamePos pos;
        pos.RAVdepth = ScidPos.RAVdepth;
        pos.RAVnum = ScidPos.RAVnum;
        pos.FEN = ScidPos.FEN;
        pos.comment = ScidPos.comment;
        pos.lastMoveSAN = ScidPos.lastMoveSAN;

        for (size_t iNag = 0, nNag = ScidPos.NAGs.size(); iNag < nNag; iNag++) {
            char temp[20];
            game_printNag(ScidPos.NAGs[iNag], temp, true, PGN_FORMAT_Plain);
            if (!pos.NAGs.empty())
                pos.NAGs << " ";
            pos.NAGs << temp;
        }

        gameLoaded->push_back(pos);
    }

    wxCommandEvent event(EVT_GAME_LOADED, wxID_ANY);
    event.SetEventObject(this);
    event.SetClientData(gameLoaded);
    ProcessEvent(event);

}
