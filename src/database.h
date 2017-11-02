/*
 * database.h
 *
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include <wx/string.h>
#include <wx/hashmap.h>
#include "scid/scid.h"

struct DbInfos
{
  wxString name;
  wxString path;
  int handle;
  int gamesNumber;
};

struct GameEntry
{
  wxString result;
  unsigned short movesNumber;
  wxString whiteName;
  wxString blackName;
  wxString whiteElo;
  wxString blackElo;
  wxString date;
  wxString eventName;
  wxString roundName;
  wxString siteName;
  unsigned int nagCount;
  unsigned int commentCount;
  unsigned int variationCount;
  char eco[6];
  char eventDate[16];
  unsigned int year;
  unsigned char rating;
  wxString firstMoves;
  unsigned int index;
};

// Declare a hash map with int keys and ScidDatabaseEntry* values
WX_DECLARE_HASH_MAP( int, GameEntry, wxIntegerHash, wxIntegerEqual, HashGameEntries);

struct ListGamesRequest
{
  int fromItem;
  int count;
  HashGameEntries *HashEntries;
};

class HashGamesPopulator : public ScidListEventHandler
{
public:
  HashGamesPopulator(HashGameEntries *hashEntries, int fromItem)
  {
    m_hashEntries = hashEntries;
    m_fromItem = fromItem;
  }

  void onListGetEntry(ScidDatabaseEntry scidEntry)
  {
    GameEntry entry;

    entry.date       = (wxString) scidEntry.date;
    entry.result     = (wxString) scidEntry.result;
    entry.whiteName  =  wxString::FromUTF8(scidEntry.white_name.c_str());
    entry.whiteElo   = (wxString) scidEntry.white_elo;
    entry.blackName  =  wxString::FromUTF8(scidEntry.black_name.c_str());
    entry.blackElo   = (wxString) scidEntry.black_elo;
    entry.firstMoves = (wxString) scidEntry.first_moves;
    entry.index      = scidEntry.index;

    (*m_hashEntries)[m_fromItem++] = entry;
  }

private:
  HashGameEntries *m_hashEntries;
  int m_fromItem;
};

#endif /* DATABASE_H_ */
