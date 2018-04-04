/*
 * GamesListView.h
 *
 *  Created on: 27 oct. 2017
 */

#ifndef WIDGETS_GAMESLISTVIEW_H_
#define WIDGETS_GAMESLISTVIEW_H_

#include <wx/event.h>
#include <wx/listctrl.h>

wxDECLARE_EVENT(EVT_DISPLAY_LIST_CELL, wxCommandEvent);

struct GameListCellData
{
    long item;
    int column;
    wxString text;
};

class GamesListCtrl: public wxListCtrl
{
public:
    enum
    {
        COL_DATE,
        COL_RESULT,
        COL_WHITENAME,
        COL_WHITEELO,
        COL_BLACKNAME,
        COL_BLACKELO,
        COL_FIRSTMOVES,
        COL_UNKNOWN,
    };

    GamesListCtrl(wxWindow *parent,
                  const wxWindowID id,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxLC_REPORT | wxLC_VIRTUAL);

private:
    virtual wxString OnGetItemText(long item, long column) const;
};

#endif /* WIDGETS_GAMESLISTVIEW_H_ */
