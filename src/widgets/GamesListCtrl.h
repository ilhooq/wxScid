/*
 * GamesListView.h
 *
 *  Created on: 27 oct. 2017
 */

#ifndef WIDGETS_GAMESLISTVIEW_H_
#define WIDGETS_GAMESLISTVIEW_H_

#include <wx/listctrl.h>
#include "database.h"

class GamesListCtrl : public wxListCtrl
{
public:
  GamesListCtrl(wxWindow *parent,
                const wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxLC_REPORT|wxLC_VIRTUAL);

private:
  HashGameEntries hashEntries;
  void OnCacheHint(wxListEvent &event);
  void OnActivated(wxListEvent &event);
  void OnOpenDatabase(wxCommandEvent& evt);
  bool CacheEntryExists(const long item) const;
  virtual wxString OnGetItemText(long item, long column) const;
wxDECLARE_EVENT_TABLE();
};

#endif /* WIDGETS_GAMESLISTVIEW_H_ */
