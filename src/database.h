/*
 * database.h
 *
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include <wx/string.h>
#include <wx/hashmap.h>

struct DbInfos
{
  wxString name;
  wxString path;
  int handle;
  int numGames;
};

struct GameEntry
{
  wxString result;
  short movesNumber;
  wxString whiteName;
  wxString blackName;
  wxString whiteElo;
  wxString blackElo;
  wxString date;
  wxString eventName;
  wxString roundName;
  wxString siteName;
  int nagCount;
  int commentCount;
  int variationCount;
  bool deletedFlag;
  wxString flags;
  wxString eco;
  wxString endMaterial;
  bool startFlag;
  wxString eventDate;
  int year;
  char rating;
  wxString firstMoves;
  int index;
  int ply;
};

struct GamePos {
  int RAVdepth;
  int RAVnum;
  wxString FEN;
  wxString NAGs;
  wxString comment;
  wxString lastMoveSAN;
};

// Declare a hash map with int as keys and GameEntry as values
WX_DECLARE_HASH_MAP( int, GameEntry, wxIntegerHash, wxIntegerEqual, HashGameEntries);

struct ListGamesRequest
{
  int fromItem;
  int count;
  HashGameEntries *HashEntries;
};

#endif /* DATABASE_H_ */
