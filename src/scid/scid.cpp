
#include <iostream>

// #include "codec_pgn.h"

#include "common.h"
#include "dbasepool.h"
#include "scidbase.h"

#include "scid.h"
/*
namespace scid {

  void print_hello(){
    std::cout<<"Hello World in C++ code\n";
  }

}
*/

Scid::Scid()
{
  DBasePool::init();
}

Scid::~Scid()
{
  DBasePool::closeAll();
}

int Scid::openDatabase(const char* filename)
{
  std::string _filename = filename;

  ICodecDatabase::Codec codec = ICodecDatabase::Codec::MEMORY;
  fileModeT fMode = FMODE_Both;

  std::string extension = _filename.substr(_filename.find_last_of(".") + 1);

  if (extension == "si4") {
    codec = ICodecDatabase::Codec::SCID4;
    // Remove the extension because scid database contains 3 files (*.si4, *.sg4, *.sn4)
    _filename = _filename.substr(0, _filename.find_last_of("."));
  } else if (extension == "pgn") {
    codec = ICodecDatabase::Codec::PGN;
  } else {
    throw ScidError("Unrecognized file format.", ERROR_CodecUnsupFeat);
  }

  if (DBasePool::find(_filename.c_str()) != 0) {
    throw ScidError("File already in use.", ERROR_FileInUse);
  }

  scidBaseT* dbase = DBasePool::getFreeSlot();

  if (dbase == 0) {
    throw ScidError("The database opening limit is reached", ERROR_Full);
  }

  Progress progress;

  errorT err = dbase->Open(codec, fMode, _filename.c_str(), progress);

  if (err != OK && err != ERROR_NameDataLoss && fMode == FMODE_Both) {
    err = dbase->Open(ICodecDatabase::Codec::SCID4, FMODE_ReadOnly, _filename.c_str(), progress);
  }

  progress.report(1,1);

  if (err != OK && err != ERROR_NameDataLoss) {
    throw ScidError("Unable to open database", err);
  }

  return DBasePool::switchCurrent(dbase);
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

  std::cout << "Games found : " << std::to_string(count) << "\n" ;

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

    entry.white_elo = eloStr.c_str();

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

    entry.black_elo = eloStr.c_str();

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

    std::string endMaterial = matsig_makeString(ie->GetFinalMatSig());
    entry.end_material = endMaterial.c_str();
    entry.start_flag = ie->GetStartFlag();

    date_DecodeToString (ie->GetEventDate(), entry.event_date);
    entry.year = ie->GetYear();
    entry.rating = ie->GetRating(nb);

    FastGame game = dbase->getGame(ie);
    entry.first_moves = game.getMoveSAN(ply, 10).c_str();
    entry.index = idx;
    entry.ply = ply;

    eventHandler->onListGetEntry(entry);
  }

  delete [] idxList;

}

