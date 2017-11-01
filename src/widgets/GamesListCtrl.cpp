/*
 * GamesListCtrl.cpp
 *
 *  Created on: 27 oct. 2017
 */

#include <wx/log.h>
#include <wx/string.h>

#include "scid/scid.h"
#include "GamesListCtrl.h"


BEGIN_EVENT_TABLE(GamesListCtrl, wxListCtrl)
  EVT_LIST_CACHE_HINT(wxID_ANY, GamesListCtrl::OnCacheHint)
  EVT_LIST_ITEM_ACTIVATED(wxID_ANY, GamesListCtrl::OnActivated)
END_EVENT_TABLE()

GamesListCtrl::GamesListCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxListCtrl(parent, id, pos, size, style)
{
  wxListItem itemCol;
  itemCol.SetText(wxT("Date"));
  itemCol.SetImage(-1);
  InsertColumn(0, itemCol);

  itemCol.SetText(wxT("Result"));
  itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
  InsertColumn(1, itemCol);

  itemCol.SetText(wxT("White"));
  itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
  InsertColumn(2, itemCol);

  itemCol.SetText(wxT("W-Elo"));
  itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
  InsertColumn(3, itemCol);

  itemCol.SetText(wxT("Black"));
  itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
  InsertColumn(4, itemCol);

  itemCol.SetText(wxT("B-Elo"));
  itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
  InsertColumn(5, itemCol);

  itemCol.SetText(wxT("Moves"));
  itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
  InsertColumn(6, itemCol);

  SetColumnWidth(0, 150);
  SetColumnWidth(1, 150);
  SetColumnWidth(2, 150);
  SetColumnWidth(3, 150);
  SetColumnWidth(4, 150);
  SetColumnWidth(5, 150);
  SetColumnWidth(6, 200);

  EnableAlternateRowColours();

  dbHandle = 0;
  scid = NULL;
}

// Called on double click or when pressed ENTER on a row
void GamesListCtrl::OnActivated(wxListEvent &event)
{
  wxASSERT(GamesListCtrl::CacheEntryExists(event.GetIndex()));
  ScidDatabaseEntry entry = hashEntries[event.GetIndex()];
  wxLogMessage(wxT("White: %s"), entry.white_name);
}

// This function is called during the window OnPaint event
void GamesListCtrl::OnCacheHint(wxListEvent& event)
{
  if (hashEntries.size() > 100000) {
    // sizeof ScidDatabaseEntry = 184 bytes
    // 100000 entries = 18.4 Mbytes
    hashEntries.clear();
  }

  if (CacheEntryExists(event.GetCacheFrom()) && CacheEntryExists(event.GetCacheTo())) {
    // Items already exists
    return;
  }

  int count = event.GetCacheTo() - event.GetCacheFrom();

  // Add padding to retrieve more items in the cache
  count += 100;

  ListGamesEventHandler eventHandler(count);

  scid->listGames(dbHandle, "d+", "dbfilter", &eventHandler, event.GetCacheFrom(), count);

  ScidDatabaseEntry *entries = eventHandler.getEntries();

  for (int i=0, item = event.GetCacheFrom(); i < count; i++, item++) {
    hashEntries[item] = *(entries+i);
  }
}

wxString GamesListCtrl::OnGetItemText(long item, long column) const
{
  wxASSERT(GamesListCtrl::CacheEntryExists(item));

  wxScidHashEntries::const_iterator it = hashEntries.find(item);
  ScidDatabaseEntry entry = it->second;

  switch(column) {
    case 0:
      return (wxString) entry.date;
    case 1:
      return (wxString) entry.result;
    case 2:
      return wxString::FromUTF8(entry.white_name.c_str());
    case 3:
      return (wxString) entry.white_elo;
    case 4:
      return wxString::FromUTF8(entry.black_name.c_str());
    case 5:
      return (wxString) entry.black_elo;
    case 6:
      return (wxString) entry.first_moves;
  }

  return wxString("Unknown column");
}

bool GamesListCtrl::CacheEntryExists(long item) const
{
  wxScidHashEntries::const_iterator it = hashEntries.find(item);
  return (it != hashEntries.end());
}
