/*
 * GamesListCtrl.cpp
 *
 *  Created on: 27 oct. 2017
 */

#include "GamesListCtrl.h"

wxDEFINE_EVENT(EVT_DISPLAY_LIST_CELL, wxCommandEvent);

GamesListCtrl::GamesListCtrl(wxWindow *parent, const wxWindowID id,const wxPoint& pos, const wxSize& size, long style) :
wxListCtrl(parent, id, pos, size, style)
{
    wxListItem itemCol;
    itemCol.SetText(wxT("Date"));
    itemCol.SetImage(-1);
    itemCol.SetAlign(wxLIST_FORMAT_LEFT);
    InsertColumn(0, itemCol);

    itemCol.SetText(wxT("Result"));
    itemCol.SetAlign(wxLIST_FORMAT_CENTER);
    InsertColumn(1, itemCol);

    itemCol.SetText(wxT("White"));
    itemCol.SetAlign(wxLIST_FORMAT_LEFT);
    InsertColumn(2, itemCol);

    itemCol.SetText(wxT("W-Elo"));
    itemCol.SetAlign(wxLIST_FORMAT_CENTER);
    InsertColumn(3, itemCol);

    itemCol.SetText(wxT("Black"));
    itemCol.SetAlign(wxLIST_FORMAT_LEFT);
    InsertColumn(4, itemCol);

    itemCol.SetText(wxT("B-Elo"));
    itemCol.SetAlign(wxLIST_FORMAT_CENTER);
    InsertColumn(5, itemCol);

    itemCol.SetText(wxT("Moves"));
    itemCol.SetAlign(wxLIST_FORMAT_LEFT);
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

wxString GamesListCtrl::OnGetItemText(long item, long column) const
{
    GameListCellData data;
    data.item = item;

    switch (column) {
        case 0:
            data.column = COL_DATE;
            break;
        case 1:
            data.column = COL_RESULT;
            break;
        case 2:
            data.column = COL_WHITENAME;
            break;
        case 3:
            data.column = COL_WHITEELO;
            break;
        case 4:
            data.column = COL_BLACKNAME;
            break;
        case 5:
            data.column = COL_BLACKELO;
            break;
        case 6:
            data.column = COL_FIRSTMOVES;
            break;
        default:
            data.column = COL_UNKNOWN;
            data.text = "Unknown column";
    }

    wxWindow *win = (wxWindow*) this;
    wxCommandEvent evt(EVT_DISPLAY_LIST_CELL, win->GetId());
    evt.SetEventObject(win);
    evt.SetClientData(&data);
    win->ProcessWindowEvent(evt);

    return data.text;
}
