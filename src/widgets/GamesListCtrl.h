/*
 * GamesListView.h
 *
 *  Created on: 27 oct. 2017
 */

#ifndef WIDGETS_GAMESLISTVIEW_H_
#define WIDGETS_GAMESLISTVIEW_H_

#include "wx/wx.h"
#include "wx/listctrl.h"
#include "scid/scid.h"

// Declare a hash map with int keys and ScidDatabaseEntry* values
WX_DECLARE_HASH_MAP( int, ScidDatabaseEntry, wxIntegerHash, wxIntegerEqual, wxScidHashEntries);

class ListGamesEventHandler : public ScidListEventHandler
{
public:
  ListGamesEventHandler(unsigned int count)
  {
    entries = new ScidDatabaseEntry[count];
    entriesStart = entries;
  }

  ~ListGamesEventHandler()
  {
    delete[] entries;
  }

  void onListGetEntry(ScidDatabaseEntry entry)
  {
    *(entriesStart++) = entry;
  }

  ScidDatabaseEntry* getEntries()
  {
    return entries;
  }

private:
  ScidDatabaseEntry *entries;
  ScidDatabaseEntry *entriesStart;
};

class GamesListCtrl : public wxListCtrl
{
public:
  GamesListCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style);

  int dbHandle;
  Scid *scid;

private:
  wxScidHashEntries hashEntries;
  void OnCacheHint(wxListEvent &event);
  void OnActivated(wxListEvent &event);
  bool CacheEntryExists(const long item) const;
  virtual wxString OnGetItemText(long item, long column) const;
wxDECLARE_EVENT_TABLE();
};



#endif /* WIDGETS_GAMESLISTVIEW_H_ */
