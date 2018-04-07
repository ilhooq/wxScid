/*
 * Scid.h
 *
 */
#ifndef SCID_H_
#define SCID_H_

#include <exception>
#include <string>
#include <wx/event.h>
#include <wx/listctrl.h>
#include <wx/vector.h>

class Scid: public wxEvtHandler
{
public:
    Scid();
    ~Scid();

private:
    int currentDbHandle;

    void LoadGame(unsigned int entryIndex);
    void openDatabase(wxCommandEvent& evt);
    void OnListItemActivated(wxListEvent &event);
    void OnListDisplayCell(wxCommandEvent& evt);
    void OnGamesListCacheHint(wxListEvent &event);
    void OnDropPiece(wxCommandEvent& evt);
    void OnMoveToPosition(wxCommandEvent& evt);

    wxDECLARE_EVENT_TABLE();
};

#endif /* SCID_H_ */
