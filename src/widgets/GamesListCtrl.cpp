/*
 * GamesListCtrl.cpp
 *
 *  Created on: 27 oct. 2017
 */

#include "GamesListCtrl.h"
#include "events.h"
#include "database.h"

BEGIN_EVENT_TABLE(GamesListCtrl, wxListCtrl)
  EVT_LIST_CACHE_HINT(wxID_ANY, GamesListCtrl::OnCacheHint)
  EVT_LIST_ITEM_ACTIVATED(wxID_ANY, GamesListCtrl::OnActivated)
  EVT_COMMAND (wxID_ANY, EVT_OPEN_DATABASE, GamesListCtrl::OnOpenDatabase)
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
}

// Called on double click or when pressed ENTER on a row
void GamesListCtrl::OnActivated(wxListEvent &event)
{
  wxASSERT(GamesListCtrl::CacheEntryExists(event.GetIndex()));
  GameEntry entry = hashEntries[event.GetIndex()];
  wxPrintf(wxT("White: %s\n"), entry.whiteName);
}

// This function is called during the window OnPaint event
void GamesListCtrl::OnCacheHint(wxListEvent& event)
{

  if (hashEntries.size() > 100000) {
    hashEntries.clear();
  }

  if (CacheEntryExists(event.GetCacheFrom()) && CacheEntryExists(event.GetCacheTo())) {
    // Items already exists
    return;
  }

  int count = event.GetCacheTo() - event.GetCacheFrom();

  // Add padding to retrieve more items in the cache
  count += 100;

  // Trigger an event to populate hashEntries outside the widget
  wxWindow *win = (wxWindow*) this;

  wxCommandEvent eventList(EVT_LISTGAMES_REQUEST, win->GetId());
  eventList.SetEventObject(this);
  ListGamesRequest data;
  data.fromItem = event.GetCacheFrom();
  data.count = count;
  data.HashEntries = &hashEntries;
  eventList.SetClientData(&data);
  win->ProcessWindowEvent(eventList);
}

wxString GamesListCtrl::OnGetItemText(long item, long column) const
{
  HashGameEntries::const_iterator it = hashEntries.find(item);

  if (it == hashEntries.end()) {
    // Entry not found
    return wxString("Error");
  }

  GameEntry entry = it->second;

  switch(column) {
    case 0:
      return entry.date;
    case 1:
      return entry.result;
    case 2:
      return entry.whiteName;
    case 3:
      return entry.whiteElo;
    case 4:
      return entry.blackName;
    case 5:
      return entry.blackElo;
    case 6:
      return entry.firstMoves;
  }

  return wxString("Unknown column");
}

bool GamesListCtrl::CacheEntryExists(long item) const
{
  HashGameEntries::const_iterator it = hashEntries.find(item);
  return (it != hashEntries.end());
}

void GamesListCtrl::OnOpenDatabase(wxCommandEvent& evt)
{
  DbInfos *infos = (DbInfos*) evt.GetClientData();
  wxPrintf(wxT("Games num: %d \n"), infos->gamesNumber);
  SetItemCount(infos->gamesNumber);
}
