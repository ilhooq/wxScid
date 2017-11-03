/*
 * Scid.cpp
 *
 */

#include "scid/scidbase.h"
#include "scid/dbasepool.h"
#include "Scid.h"
#include "events.h"
#include "database.h"

BEGIN_EVENT_TABLE(Scid, wxEvtHandler)
EVT_COMMAND (wxID_ANY, EVT_OPEN_DATABASE_REQUEST, Scid::openDatabase)
EVT_COMMAND (wxID_ANY, EVT_LISTGAMES_REQUEST, Scid::OnListGames)
END_EVENT_TABLE()

Scid::Scid()
{
  DBasePool::init();
  currentDbHandle = 0;
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

  progress.report(1,1);

  if (err != OK && err != ERROR_NameDataLoss) {
    throw ScidError("Unable to open database", err);
  }

  currentDbHandle = DBasePool::switchCurrent(dbase);

  DbInfos infos;
  infos.handle = currentDbHandle;
  infos.gamesNumber = dbase->numGames();
  infos.path = path;

  wxCommandEvent event(EVT_OPEN_DATABASE, wxID_ANY);
  event.SetEventObject(this);
  event.SetClientData(&infos);
  ProcessEvent(event);
}

void Scid::OnListGames(wxCommandEvent& evt)
{
  ListGamesRequest *data = (ListGamesRequest*) evt.GetClientData();
  HashGamesPopulator populator(data->HashEntries, data->fromItem);
  listGames(currentDbHandle, "d+", "dbfilter", &populator, data->fromItem, data->count);
}

void Scid::listGames(int baseHandle, const char* ordering, const char* filterId, ScidListEventHandler* eventHandler, unsigned int start, unsigned int count)
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

  for (uint i = 0; i < count; ++i) {

    uint idx = idxList[i];
    ASSERT(filter->get(idx) != 0);
    ScidDatabaseEntry entry;

    uint ply = filter->get(idx) -1;

    const IndexEntry* ie = dbase->getIndexEntry(idx);

    entry.result = RESULT_STR[ie->GetResult()];
    entry.moves_number = (ie->GetNumHalfMoves() + 1) / 2;
    entry.white_name = ie->GetWhiteName(nb);
    entry.black_name = ie->GetBlackName(nb);

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

    entry.white_elo = eloStr;

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

    entry.black_elo = eloStr;

    date_DecodeToString(ie->GetDate(), entry.date);
    entry.event_name = ie->GetEventName(nb);
    entry.round_name = ie->GetRoundName(nb);
    entry.site_name = ie->GetSiteName(nb);
    entry.nag_count = ie->GetNagCount();
    entry.comment_count = ie->GetCommentCount();
    entry.variation_count = ie->GetVariationCount();
    entry.deleted_flag = ie->GetDeleteFlag();
    ie->GetFlagStr (entry.flags, "WBMENPTKQ!?U123456");
    eco_ToExtendedString(ie->GetEcoCode(), entry.eco);
    entry.end_material = matsig_makeString(ie->GetFinalMatSig());
    entry.start_flag = ie->GetStartFlag();
    date_DecodeToString (ie->GetEventDate(), entry.event_date);
    entry.year = ie->GetYear();
    entry.rating = ie->GetRating(nb);
    FastGame game = dbase->getGame(ie);
    entry.first_moves = game.getMoveSAN(ply, 10);
    entry.index = idx;
    entry.ply = ply;

    eventHandler->onListGetEntry(entry);
  }

  delete [] idxList;

}

unsigned int Scid::numGames(int baseHandle)
{
  scidBaseT* dbase = DBasePool::getBase(baseHandle);
  return dbase->numGames();
}


