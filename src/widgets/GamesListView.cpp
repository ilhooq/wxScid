/*
 * GamesListView.cpp
 *
 *  Created on: 27 oct. 2017
 */
#include "wx/wx.h"
#include "GamesListView.h"
#include "scid/scid.h"


BEGIN_EVENT_TABLE(GamesListView, wxListCtrl)
  EVT_LIST_CACHE_HINT(wxID_ANY, GamesListView::OnCacheHint)
END_EVENT_TABLE()

GamesListView::GamesListView(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
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

  itemCol.SetText(wxT("Black"));
  itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
  InsertColumn(3, itemCol);

  SetColumnWidth(0, 150);
  SetColumnWidth(1, 150);
  SetColumnWidth(2, 150);
  SetColumnWidth(3, 150);

  // SetItemCount(500);
  EnableAlternateRowColours();
  dbHandle = 0;
  scid = NULL;
}

void GamesListView::OnCacheHint(wxListEvent& event)
{
  wxPrintf(wxT("OnCacheHint: cache items %ld..%ld \n"), event.GetCacheFrom(), event.GetCacheTo());

  if (CacheEntryExists(event.GetCacheFrom()) && CacheEntryExists(event.GetCacheTo())) {
    // Items already exist
    RefreshItems(event.GetCacheFrom(), event.GetCacheTo());
    return;
  }

  unsigned int count = event.GetCacheTo() - event.GetCacheFrom();

  ListGamesEventHandler eventHandler(count);

  scid->listGames(dbHandle, "d+", "dbfilter", &eventHandler, event.GetCacheFrom(), count);

  ScidDatabaseEntry *entries = eventHandler.getEntries();

  for (int i=0, item = event.GetCacheFrom(); i < count; i++, item++) {
    hashEntries[item] = *(entries+i);
    wxPrintf(wxT("Entry %d - Index: %d - Ply: %d - White name: %s\n"),i, hashEntries[item].index, hashEntries[item].ply, (wxString) hashEntries[item].white_name);
  }
}

wxString GamesListView::OnGetItemText(long item, long column) const
{
  bool found = GamesListView::CacheEntryExists(item);

  if (!found) {
    return wxString::Format(wxT("Column %ld of item %ld not found"), column, item);
  }

  wxScidHashEntries::const_iterator it = hashEntries.find(item);
  ScidDatabaseEntry entry = it->second;

  switch(column) {
    case 0:
      return (wxString) entry.date;
    case 1:
      return (wxString) entry.result;
    case 2:
      return (wxString) entry.white_name;
    case 3:
      return (wxString) entry.black_name;
    default:
      return wxString("Unknown column");
  }
}

bool GamesListView::CacheEntryExists(long item) const
{
  wxScidHashEntries::const_iterator it = hashEntries.find(item);
  return (it != hashEntries.end());
}
